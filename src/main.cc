#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <GL/glfw.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <mesh/mesh.h>

#include "solid.h"
#include "surface_coordinate.h"
#include "particle.h"
#include "player.h"
#include "puzzle.h"
#include "entity.h"
#include "sound.h"
#include "text.h"
#include "menu.h"
#include "assets.h"

using namespace std;
using namespace Eigen;
using namespace Mesh;

namespace App {

int start_level = -1;
int cur_level = -1;

bool running = true;
bool ingame = false;
bool dead = false;
bool winner = false;

double fov=45., znear=1., zfar=1000.;
Puzzle puzzle;
	
Menu* mainmenu;
Menu* quitmenu;
Menu* levelmenu;
Menu* optionsmenu;
	
Menu* gamemenu;
Menu* gamequitmenu;
	
//currently displayed menu
Menu* currentmenu;

//only care about up, down, enter, and escape for menus
#define MENU_KEY_UP 0
#define MENU_KEY_DOWN 1
#define MENU_KEY_ESC 2
#define MENU_KEY_ENTER 3
#define MENU_KEY_KP_ENTER 4
bool menukeys[5];

void exit(void* data)
{
	running = false;
}

void showmenu(void* data)
{
	if(currentmenu == NULL && data != NULL)
		play_sound_from_group(SOUND_GROUP_MENU_SHOW);
	
	currentmenu = (Menu*)data;
	
	if(currentmenu != NULL)
		currentmenu->reset();
}

int getlevelindex(int i)
{
	switch(i)
	{
		case 0:
			return 0;
		case 1:
			return 3;
		case 2:
			return 4;
		case 3:
			return 2;
		case 4:
			return 1;
	}
	
	return 0;
}

void endgame(void* data)
{
	ingame = false;
	showmenu(mainmenu);
}

void endgame2(void* data)
{
	endgame(data);
	dead = false;
}

void startlevel(void* data)
{
	int lev = (int)data;

	dead = false;
	winner = false;
	cur_level = lev;
	
	puzzle.setup(get_level(getlevelindex(lev)));
	ingame = true;
	
	showmenu(NULL);
}
	
void init()
{
	//initialize random seed
	srand(time(NULL));
	
	//Initialize artwork
	init_assets();
	
	//initialize text stuff
	initialize_text();
	
	for(int x = 0; x < 4; x++)
		menukeys[x] = false;
	
	//set up main menu
	mainmenu = new Menu("Main menu");
	quitmenu = new Menu("Really quit?");
	levelmenu = new Menu("Level Select");
	optionsmenu = new Menu("Options");
	
	mainmenu->add_option("New Game", &startlevel, (void*)0);
	//mainmenu->add_option("Options", &showmenu, optionsmenu);
	mainmenu->add_option("Exit", &showmenu, quitmenu);
	mainmenu->set_esc_option(1);
	
	quitmenu->add_option("No", &showmenu, mainmenu);
	quitmenu->add_option("Yes", &exit);
	quitmenu->set_esc_option(0);
	
	levelmenu->add_option("Level 1", &startlevel, (void*)0);
	levelmenu->add_option("Level 2", &startlevel, (void*)1);
	levelmenu->add_option("Level 3", &startlevel, (void*)2);
	levelmenu->add_option("Level 4", &startlevel, (void*)3);
	levelmenu->add_option("Level 5", &startlevel, (void*)4);
	levelmenu->add_option("Go Back", &showmenu, mainmenu);
	levelmenu->set_esc_option(6);
	
	optionsmenu->add_option("Go Back", &showmenu, mainmenu);
	optionsmenu->set_esc_option(0);
	
	//set up in-game menu
	gamemenu = new Menu("Select Action (paused)");
	gamequitmenu = new Menu("Really quit?");

	gamemenu->add_option("Return to game", &showmenu, NULL);
	gamemenu->add_option("End game", &endgame2);
	gamemenu->add_option("Exit", &showmenu, gamequitmenu);
	gamemenu->set_esc_option(0);

	//duplicated the quit menu. It would be nice if I could reuse quitmenu, but it's a weakness of this menu design (it's quick and easy to code)
	gamequitmenu->add_option("No", &showmenu, gamemenu);
	gamequitmenu->add_option("Yes", &exit);
	gamequitmenu->set_esc_option(0);

	//teleporter sounds
	load_sound_in_group("data/teleport1.wav", SOUND_GROUP_TELEPORT);
	load_sound_in_group("data/teleport2.wav", SOUND_GROUP_TELEPORT);
	load_sound_in_group("data/teleport3.wav", SOUND_GROUP_TELEPORT);
	
	//menu sounds
	load_sound_in_group("data/menuoption.wav", SOUND_GROUP_MENU_CHANGE);
	load_sound_in_group("data/menuselect.wav", SOUND_GROUP_MENU_SELECT);
	load_sound_in_group("data/showmenu.wav", SOUND_GROUP_MENU_SHOW);
	
	//button sounds
	load_sound_in_group("data/button1.wav", SOUND_GROUP_BUTTON);
	load_sound_in_group("data/button2.wav", SOUND_GROUP_BUTTON);
	load_sound_in_group("data/button3.wav", SOUND_GROUP_BUTTON);
	load_sound_in_group("data/button4.wav", SOUND_GROUP_BUTTON);
	
	//gate closing
	load_sound_in_group("data/gateclose.wav", SOUND_GROUP_GATE_CLOSE);
	
	//bounce
	load_sound_in_group("data/bounce1.wav", SOUND_GROUP_BOUNCE);
	//load_sound_in_group("data/bounce2.wav", SOUND_GROUP_BOUNCE);
	load_sound_in_group("data/bounce3.wav", SOUND_GROUP_BOUNCE);
	load_sound_in_group("data/bounce4.wav", SOUND_GROUP_BOUNCE);
	load_sound_in_group("data/bounce5.wav", SOUND_GROUP_BOUNCE);
	
	//death
	load_sound_in_group("data/death.wav", SOUND_GROUP_DEATH);
	
	//ticking
	load_sound_in_group("data/tick_low.wav", SOUND_GROUP_TICK_LOW);
	load_sound_in_group("data/tick_high.wav", SOUND_GROUP_TICK_HIGH);
	
	//initialize level data
	if(start_level != -1)
		startlevel((void*)start_level);
	else
		showmenu(mainmenu);
}

bool togglekey(int GLFWKey, int menukey)
{
	bool b = (glfwGetKey(GLFWKey) == GLFW_PRESS);
	
	if(b && !menukeys[menukey])
	{
		if(currentmenu != NULL)
		{
			switch(menukey)
			{
				case MENU_KEY_UP:
					currentmenu->prev_option();
					break;
				case MENU_KEY_DOWN:
					currentmenu->next_option();
					break;
				case MENU_KEY_ESC:
					currentmenu->activate_esc_option();
					break;
				case MENU_KEY_ENTER:
				case MENU_KEY_KP_ENTER:
					currentmenu->select_option();
					break;
				default:
					break;
			}
		}
		else
		{
			showmenu(gamemenu);
		}
	}
	
	menukeys[menukey] = b;
}

void input()
{
    if(!glfwGetWindowParam(GLFW_OPENED))
	{
        running = false;
		return;
    }
	
	togglekey(GLFW_KEY_UP, MENU_KEY_UP);
	togglekey(GLFW_KEY_DOWN, MENU_KEY_DOWN);
	togglekey(GLFW_KEY_ESC, MENU_KEY_ESC);
	togglekey(GLFW_KEY_ENTER, MENU_KEY_ENTER);
	togglekey(GLFW_KEY_KP_ENTER, MENU_KEY_KP_ENTER);
    
	//if there is no menu showing, then go ahead any do player input
	if(currentmenu == NULL)
		puzzle.input();
}

void tick()
{
	static double last_t = 0.0;
	auto t = glfwGetTime();
	auto dt = t - last_t;
	last_t = t;
	
	//if there is no menu showing, then tick the game forward. Otherwise, don't tick (so the game is paused)
	if(currentmenu == NULL)
	{
		puzzle.tick(dt);
		
		if(puzzle.level_complete) {
			if(cur_level < 4)
				startlevel((void*)(cur_level + 1));
			else
			{
				winner = true;
				endgame(NULL);
			}
		}
	}
}

void draw() {

	int w, h;
	glfwGetWindowSize(&w, &h);
	glViewport(0, 0, w, h);
	
	glClearColor(0.3, 0.3, 0.8, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//only draw the level and entities, etc. if we are currenty in the game
	if(ingame)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);

		//Set up projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fov, (float)w / (float)h, znear, zfar);
		
		glMatrixMode(GL_MODELVIEW);
		
		//Draw the level
		puzzle.draw();
	}

	//text rendering should be the last thing we do in the render loop
	start_text_rendering();
	
	//draw text as white
	glColor3f(1, 1, 1);
	
	if(winner)
	{
		float size = 0.1;
		const char* text = "You Escaped!";
		
		show_text(text, (1 - text_width(text, size)) / 2, 0.8, size, TEXT_STYLE_NORMAL);
	}
	
	if(currentmenu != NULL)
		currentmenu->render();
	
	end_text_rendering();
}


};

int initialize_libs()
{
    glfwInit();
    if (!glfwOpenWindow(640,480, 8, 8, 8, 8, 16, 0, GLFW_WINDOW))
        return 0;

	if(!initialize_sound_driver())
		return 0;
	
	return 1;
}

int main(int argc, char* argv[])
{
	if(!initialize_libs())
	{
		glfwTerminate();
		return -1;
	}
	
	if(argc > 1) {
		App::start_level = atoi(argv[1]);
	}
	
    glfwSetWindowTitle("Help!  I'm stuck in a compact 2D Riemannian manifold!");
    
    App::init();
    while(App::running) {
        App::input();
        App::draw();
        App::tick();
        glfwSwapBuffers();
    }

    glfwTerminate();
    return 0;
}

