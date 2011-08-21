#ifndef SOUND_H
#define SOUND_H

#include <vector>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#define SOUND_FREQ_44KHZ 44100
#define SOUND_FREQ_22KHZ 22050
#define SOUND_FREQ_11KHZ 11025

#define MAX_STREAMS 32

int initialize_sound_driver();
bool set_sound_format(int freq, bool stereo);
int load_sound(char* path);
int play_sound(int i);

class Sound;
class AudioStream;

class Sound
{
	public:
		Sound(char* path);
		void set_format(SDL_AudioSpec,AudioStream**);
		~Sound();
	
		//converted sound variables
		SDL_AudioCVT cnv;
		SDL_AudioSpec cnvfmt;

		//this is just to track sound objects with errors
		bool valid;
	
	private:
		//unconverted sound variables
		SDL_AudioSpec fmt;
		Uint8 *data;
		Uint32 dlen;
	
		void cleanup_converted_data();
};

class AudioStream
{
	public:
		AudioStream();
		Sound* sound;
		bool playing;
		Uint32 pos;
		int index;
		Uint8* data();
		Uint32 len();
};

class AudioDriver
{
	public:
		void initialize();
		void unload();
		bool set_format(int freq, bool stereo);
		void mix_audio(Uint8 *stream, int len);
		int load_sound_from_file(char* path);
		int play_sound_object(int i);
		void stop_stream(int i);
	
	private:
		bool loaded;
		bool enabled;
	
		int streamindex;
	
		SDL_AudioSpec curfmt;
	
		int get_free_stream();
	
		std::vector<Sound*> sounds;
		AudioStream* streams[MAX_STREAMS];
};

#endif
