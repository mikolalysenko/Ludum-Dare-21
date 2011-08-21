#include "sound.h"

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

int load_sound(char* path)
{
	return driver.load_sound_from_file(path);
}

int play_sound(int i)
{
	return driver.play_sound_object(i);
}

int initialize_sound_driver()
{
	driver.initialize();
	
	int sf = load_sound("data/audio.wav");
	
	if(!driver.set_format(SOUND_FREQ_22KHZ, true))
	{
		//TODO: get error from SDL if an error happened
		//error = SDL_GetError()
		
		return 0;
	}
	
	int s = play_sound(sf);
	
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

Sound::Sound(char* path)
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

int AudioDriver::load_sound_from_file(char* path)
{
	Sound* s = new Sound(path);
	
	//do the proper sound format conversion
	s->set_format(curfmt, streams);
	
	sounds.push_back(s);
	
	return sounds.size();
}

void AudioDriver::mix_audio(Uint8 *stream, int len)
{
    Uint32 amount;

    for(int i = 0; i < MAX_STREAMS; i++)
	{
		if(streams[i]->playing)
		{
			amount = (streams[i]->len() - streams[i]->pos);
			if(amount > len)
			{
				amount = len;
			}
			SDL_MixAudio(stream, &streams[i]->data()[streams[i]->pos], amount, SDL_MIX_MAXVOLUME);
			streams[i]->pos += amount;
		}
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

int AudioDriver::play_sound_object(int i)
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
	streams[stream]->playing = true;
	streams[stream]->index = streamindex;
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
