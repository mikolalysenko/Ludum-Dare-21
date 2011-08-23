#ifndef SOUND_H
#define SOUND_H

#include <vector>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#define SOUND_FREQ_44KHZ 44100
#define SOUND_FREQ_22KHZ 22050
#define SOUND_FREQ_11KHZ 11025

#define MAX_STREAMS 32

#define SOUND_GROUP_TELEPORT 0
#define SOUND_GROUP_MENU_CHANGE 1
#define SOUND_GROUP_MENU_SELECT 2
#define SOUND_GROUP_MENU_SHOW 3
#define SOUND_GROUP_BUTTON 4
#define SOUND_GROUP_GATE_CLOSE 5
#define SOUND_GROUP_BOUNCE 6
#define SOUND_GROUP_DEATH 7
#define SOUND_GROUP_TICK_LOW 8
#define SOUND_GROUP_TICK_HIGH 9
#define SOUND_GROUP_LAST 10

int initialize_sound_driver();
bool set_sound_format(int freq, bool stereo);
int load_sound(const char* path);
int load_sound_in_group(const char* path, int group);
int play_sound_from_group(int group, bool looping = false, float rate = 1);
int play_sound(int i, bool looping = false, float rate = 1);
void update_rate(int stream, float rate);

class Sound;
class AudioStream;

class Sound
{
	public:
		Sound(const char* path);
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
		float pos;
		float rate;
		bool looping;
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
		int load_sound_from_file(const char* path);
		int play_sound_object(int i, bool looping = false, float rate = 1);
		void stop_stream(int i);
		void update_stream_rate(int stream, float rate);
	
	private:
		bool loaded;
		bool enabled;
	
		int streamindex;
	
		SDL_AudioSpec curfmt;
	
		int get_free_stream();
		int get_stream_from_index(int);
	
		std::vector<Sound*> sounds;
		AudioStream* streams[MAX_STREAMS];
};

#endif
