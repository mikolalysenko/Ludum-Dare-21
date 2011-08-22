#include "sound.h"

#include <stdlib.h>

std::vector<std::vector<int>> soundgroup(SOUND_GROUP_LAST);

int load_sound_in_group(const char* path, int group)
{
	int sound = load_sound(path);
	soundgroup[group].push_back(sound);
	
	return sound;
}

int play_sound_from_group(int group, bool looping, float rate)
{
	int i = rand();
	int num = i % soundgroup[group].size();
	
	return play_sound(soundgroup[group][num], looping, rate);
}

AudioDriver driver;

void mix_audio_c(void *drv, Uint8 *stream, int len)
{
	//not really necessary, since driver is a singleton class and it's defined above, but I'm doing this anyway
	AudioDriver* d = (AudioDriver*)drv;
	d->mix_audio(stream, len);
}

bool set_sound_format(int freq, bool stereo)
{
	return driver.set_format(freq, stereo);
}

int load_sound(const char* path)
{
	return driver.load_sound_from_file(path);
}

int play_sound(int i, bool looping, float rate)
{
	return driver.play_sound_object(i, looping, rate);
}

int initialize_sound_driver()
{
	driver.initialize();
	
	if(!driver.set_format(SOUND_FREQ_22KHZ, true))
	{
		//TODO: get error from SDL if an error happened
		//error = SDL_GetError()
		
		return 0;
	}
	
	return 1;
}

AudioStream::AudioStream()
{
	playing = false;
}

Uint8* AudioStream::data()
{
	return sound->cnv.buf;
}

Uint32 AudioStream::len()
{
	return sound->cnv.len;
}

Sound::~Sound()
{
	if(!valid)
		return;
	
	SDL_FreeWAV(data);
	
	cleanup_converted_data();
}

void Sound::cleanup_converted_data()
{
	if(cnv.buf)
	{
		free(cnv.buf);
		cnv.buf = NULL;
	}
}

void Sound::set_format(SDL_AudioSpec newfmt, AudioStream** streams)
{
	//if there was an error during load, nothing to do here
	if(!valid)
		return;
	
	if(newfmt.freq == 0)
		return;
	
	//we are asking it to convert from a format into the same format, nothing to do here
	if(newfmt.format == cnvfmt.format && newfmt.channels == cnvfmt.channels && newfmt.freq == cnvfmt.freq)
		return;
	
	SDL_AudioCVT newcnv;
	
	SDL_BuildAudioCVT(&newcnv, fmt.format, fmt.channels, fmt.freq, newfmt.format, newfmt.channels, newfmt.freq);
	newcnv.buf = (Uint8*)malloc(dlen * newcnv.len_mult);
	memcpy(newcnv.buf, data, dlen);
	newcnv.len = dlen;
	SDL_ConvertAudio(&newcnv);

	//clean up all the old converted data, since we don't need it anymore
	cleanup_converted_data();
	
	//copy the converted data over
	cnv = newcnv;
	cnvfmt = newfmt;
	
	//TODO: scale the current position of the streams by the change in format length. That way, we can maintain continuity of sound even though the format changes
}

Sound::Sound(const char* path)
{
	valid = false;
	data = NULL;
	cnv.buf = NULL;
	
	int index;
	SDL_AudioCVT cvt;
	
	//Load the data from the sound file and cache it in the local unconverted data
	if (SDL_LoadWAV(path, &fmt, &data, &dlen) == NULL)
	{
		fprintf(stderr, "Couldn't load %s: %s\n", path, SDL_GetError());
		return;
	}
	
	valid = true;
}

void AudioDriver::stop_stream(int i)
{
	for(int x = 0; x < MAX_STREAMS; x++)
	{
		if(streams[x]->index == i)
			streams[x]->playing = false;
	}
}

int AudioDriver::load_sound_from_file(const char* path)
{
	Sound* s = new Sound(path);
	
	//do the proper sound format conversion
	s->set_format(curfmt, streams);
	
	sounds.push_back(s);
	
	return sounds.size();
}

int interpolate(float val, int min, int max)
{
	val -= (int)val;
	
	if(val == 0)
		return min;
	
	return (int)((min * (1 - val)) + (max * val));
}

void AudioDriver::mix_audio(Uint8 *stream, int len)
{
    Uint32 amount;

	len >>= 1;
	
    for(int i = 0; i < MAX_STREAMS; i++)
	{
		if(!streams[i]->playing)
			continue;
		
		short *curstream = (short*)stream;
		int index = 0;
		short *readstream = (short*)streams[i]->data();
		int readlen = streams[i]->len() >> 1;
		float pos = streams[i]->pos;
		
		while(index < len && streams[i]->playing)
		{
			int ipos = (int)pos;
			int left, right;
			left = readstream[ipos];
			if(ipos == readlen - 1)
				right = readstream[ipos];
			else
				right = readstream[ipos+1];
			
			int nval = curstream[index];
			nval += interpolate(pos, left, right);
			if(nval > 0x7fff)
				nval = 0x7fff;
			if(nval < -(0x8000))
				nval = 0x8000;
			curstream[index] = nval;
			
			pos += streams[i]->rate;
			
			if(pos >= readlen)
			{
				if(streams[i]->looping)
				{
					while(pos >= readlen)
						pos -= readlen;
				}
				else
					streams[i]->playing = false;
			}
			
			index++;
		}
		streams[i]->pos = pos;
    }
}

void AudioDriver::initialize()
{
	loaded = false;
	curfmt.freq = 0;
	streamindex = 1;
	
	for(int x = 0; x < MAX_STREAMS; x++)
		streams[x] = new AudioStream();
}

void AudioDriver::unload()
{
	if(!loaded)
		return;
	
	SDL_CloseAudio();
	loaded = false;
}

int AudioDriver::get_free_stream()
{
	for(int x = 0; x < MAX_STREAMS; x++)
	{
		if(!streams[x]->playing)
			return x;
	}
	
	return -1;
}

int AudioDriver::play_sound_object(int i, bool looping, float rate)
{
	int stream = get_free_stream();
	
	//too many sounds playing right now, just skip this one
	if(stream == -1)
		return -1;
	
	i--;  //returned sound indexes are 1-indexed (to avoid complications with uninitialized data), so subtract one from the passed in index
	
	//didn't specify a valid index
	if(i == -1 || i >= sounds.size())
		return -1;
	
	//the user picked a sound that had an error during load
	if(!sounds[i]->valid)
		return -1;
	
	SDL_LockAudio();
	streams[stream]->sound = sounds[i];
	streams[stream]->pos = 0;
	streams[stream]->rate = rate;
	streams[stream]->playing = true;
	streams[stream]->index = streamindex;
	streams[stream]->looping = looping;
	SDL_UnlockAudio();
	
	streamindex++;
	
	return streams[stream]->index;
}

bool AudioDriver::set_format(int freq, bool stereo)
{
	//make sure the audio driver is unloaded before we try this
	unload();
	
	//build new format structure
	SDL_AudioSpec fmt;

    fmt.freq = freq;
    fmt.format = AUDIO_S16;
	
	if(stereo)
		fmt.channels = 2;
	else
		fmt.channels = 1;
	
    fmt.samples = 512;
    fmt.callback = mix_audio_c;
    fmt.userdata = this;

    //open audio for playback
    if(SDL_OpenAudio(&fmt, NULL) < 0)
		return false;
	
	SDL_PauseAudio(0);  //make sure audio can play
	
	curfmt = fmt;
	loaded = true;
	
	//make sure we push the sound format out to all the cached sound objects
	for(int x = 0; x < sounds.size(); x++)
		sounds[x]->set_format(curfmt, streams);
	
	return true;
}
