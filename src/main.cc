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
#include "sound.h"
#include "text.h"
#include "menu.h"
#include "art.h"

#include "levels/level0.h"

using namespace std;
using namespace Eigen;
using namespace Mesh;

namespace App {

bool running = true;
bool ingame = true;

GLuint scene_display_list = 0;
double fov=45., znear=1., zfar=1000.;

//vector<Particle> particles;
Player player;
Puzzle puzzle;
	
Menu* mainmenu;
Menu* quitmenu;
Menu* menu1;
Menu* menu2;
Menu* menu3;
	
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
	currentmenu = (Menu*)data;
	
	if(currentmenu != NULL)
		currentmenu->reset();
}

void endgame(void* data)
{
	ingame = false;
	showmenu(mainmenu);
}
	
void init()
{
	//initialize random seed
	srand(time(NULL));
	
	//Initialize artwork
	init_artwork();
	
	//initialize text stuff
	initialize_text();
	
	for(int x = 0; x < 4; x++)
		menukeys[x] = false;
	
	//set up main menu
	mainmenu = new Menu("Main menu");
	quitmenu = new Menu("Really quit?");
	menu1 = new Menu("New menu for option 1");
	menu2 = new Menu("New menu for option 2");
	menu3 = new Menu("New menu for option 3");
	
	mainmenu->add_option("Option 1", &showmenu, menu1);
	mainmenu->add_option("Option 2", &showmenu, menu2);
	mainmenu->add_option("Option 3", &showmenu, menu3);
	mainmenu->add_option("Exit", &showmenu, quitmenu);
	mainmenu->set_esc_option(3);
	
	quitmenu->add_option("No", &showmenu, mainmenu);
	quitmenu->add_option("Yes", &exit);
	quitmenu->set_esc_option(0);
	
	menu1->add_option("Go Back", &showmenu, mainmenu);
	menu1->set_esc_option(0);
	
	menu2->add_option("This doesn't do anything either!", &showmenu, mainmenu);
	menu2->set_esc_option(0);

	menu3->add_option("Try this one (goes back)", &showmenu, mainmenu);
	menu3->set_esc_option(0);
	
	//set up in-game menu
	gamemenu = new Menu("In-Game menu (paused)");
	gamequitmenu = new Menu("Really quit?");

	gamemenu->add_option("Return to game", &showmenu, NULL);
	gamemenu->add_option("End game", &endgame);
	gamemenu->add_option("Exit", &showmenu, gamequitmenu);
	gamemenu->set_esc_option(0);

	//duplicated the quit menu. It would be nice if I could reuse quitmenu, but it's a weakness of this menu design (it's quick and easy to code)
	gamequitmenu->add_option("No", &showmenu, gamemenu);
	gamequitmenu->add_option("Yes", &exit);
	gamequitmenu->set_esc_option(0);

	//TODO: in the future, this should be "showmenu(mainmenu)". Right now, I am defaulting it to just jump right into the game to make development easier (so you don't have to go through menus to test)
	showmenu(NULL);

	//initialize level data
	Level0::Level0 level_gen;
	setup_puzzle(puzzle, level_gen, &player);
	
	/*
	//Generate a bunch of random particles
	for(int i=0; i<100; ++i) {
		int t = rand() % puzzle.mesh.triangles().size();
		particles.push_back(
			Particle(
				IntrinsicCoordinate(t,
					puzzle.mesh.vertex(puzzle.mesh.triangle(t).v[rand()%3]).position,
					&puzzle),
				10.*Vector3f(0.5-drand48(), 0.5-drand48(), 0.5-drand48()),
				Vector3f(drand48(), drand48(), drand48()),
				(float)(drand48()*10.f) ));
	}
	*/
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
		player.input();
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
		/*
		for(int i=0; i<particles.size(); ++i) {
			particles[i].integrate(dt);
		}
		*/
		
		player.tick(dt);
		puzzle.tick(dt);
	}
}

void draw() {

	glClearColor(0.3, 0.3, 0.8, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//only draw the level and entities, etc. if we are currenty in the game
	if(ingame)
	{
		int w, h;
		glfwGetWindowSize(&w, &h);
		glViewport(0, 0, w, h);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);

		//Set up projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fov, (float)w / (float)h, znear, zfar);
		
		glMatrixMode(GL_MODELVIEW);
		
		//Set camera
		player.set_gl_matrix();
		
		//Draw the level
		puzzle.draw();
		
		//Draw the player
		player.draw();
		
			/*
		//Draw particles
		glPointSize(5);
		glBegin(GL_POINTS);
		for(int i=0; i<particles.size(); ++i) {
			auto p = particles[i];
			glColor3f(p.color[0], p.color[1], p.color[2]);
			glVertex3f(p.coordinate.position[0], p.coordinate.position[1], p.coordinate.position[2]);
		}
		glEnd();
		*/
	}

	//text rendering should be the last thing we do in the render loop
	start_text_rendering();
	
	//draw text as white
	glColor3f(1, 1, 1);
	
	/*show_text("Normal Text!", 0, 0, 0.05, TEXT_STYLE_NORMAL);
	show_text("Bold Text!", 0, 0.05, 0.05, TEXT_STYLE_BOLD);
	show_text("Italic Text!", 0, 0.1, 0.05, TEXT_STYLE_ITALIC);
	show_text("Bold and Italic Text!", 0, 0.15, 0.05, TEXT_STYLE_BOLD_AND_ITALIC);
	show_text("Small Text!", 0, 0.2, 0.02);
	show_text("Big Text!", 0, 0.22, 0.08);
	
	show_text("Right Text!", 1 - text_width("Right Text!", 0.05), .5, 0.05, TEXT_STYLE_NORMAL);
	show_text("Center Text!", (1 - text_width("Center Text!", 0.05)) / 2, .95, 0.05, TEXT_STYLE_NORMAL);
	
	char allstr[96];
	
	for(int x = 0; x < 95; x++)
		allstr[x] = x + 32;

	allstr[95] = '\0';
	
	show_text(allstr, 0, 0.8, 0.02);*/
	
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

