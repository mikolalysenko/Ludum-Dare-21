#include "menu.h"

#include "text.h"

#include <GL/glfw.h>

Menu::Menu(const char* t)
{
	title = t;
}

void Menu::set_esc_option(int i)
{
	escoption = i;
}

void Menu::activate_esc_option()
{
	curoption = escoption;
	select_option();
}

void Menu::next_option()
{
	curoption++;
	if(curoption >= options.size())
		curoption = options.size() - 1;
}

void Menu::prev_option()
{
	curoption--;
	if(curoption < 0)
		curoption = 0;
}

void Menu::select_option()
{
	//don't do anything out of bounds
	if(curoption < 0 || curoption > options.size())
		return;
	
	if(options[curoption].cb != NULL)
	{
		options[curoption].cb(options[curoption].data);
	}
}

void Menu::reset()
{
	curoption = 0;
}

void Menu::render()
{
	//this block of code is dedicated to darkening the background so that the menu is more easily visible. I have no idea if this is the best way to do this.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 1, 0, 1);  //we just want to put a quad over the whole screen, so set up simple 0-1 bounds
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			
			//GL_COLOR_BUFFER_BIT may be enough here, but I'm not sure if the glEnable will get popped, so I am pushing all attrib bits
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
				glColor4f(0, 0, 0, .5);
	
				glBegin(GL_QUADS);
	
					glVertex2i(0, 1);
					glVertex2i(1, 1);
					glVertex2i(1, 0);
					glVertex2i(0, 0);
					
				glEnd();
	
			glPopAttrib();
		
		glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	
	float xp = .3;
	float yp = .7;
	float size = .05;
	float padding = 0.01;
	
	float optx, opty;
	
	show_text(title, xp, yp, size, TEXT_STYLE_BOLD);
	yp -= size + padding;
	
	for(int x = 0; x < options.size(); x++)
	{
		if(x == curoption)
		{
			optx = xp;
			opty = yp;
		}
		
		show_text(options[x].option, xp, yp, size);
		yp -= size + padding;
	}
	
	show_text(">", optx - .05, opty, size);
}

void Menu::add_option(const char* text, void (*func)(void*), void* data)
{
	MenuOption o;
	
	o.option = text;
	o.cb = func;
	o.data = data;
	
	options.push_back(o);
}