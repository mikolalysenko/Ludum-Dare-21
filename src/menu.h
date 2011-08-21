#ifndef MENU_H
#define MENU_H

#include <vector>

typedef struct _MenuOption
{
	const char* option;
	void (*cb)(void*);
	void* data;
} MenuOption;

class Menu
{
	public:
		Menu(const char*);
		void add_option(const char* text, void (*func)(void*), void* data=NULL);
		void render();
		void reset();
		void set_esc_option(int i);
		void next_option();
		void prev_option();
		void select_option();
		void activate_esc_option();
	
	private:
		const char* title;
		std::vector<MenuOption> options;
		int curoption;
		int escoption;
};

#endif