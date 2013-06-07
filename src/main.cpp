
#include "input.h"
#include "scene/scene.h"

#include "leap/Leap.h"

#include <SDL/SDL.h>
//#include <SDL_ttf/SDL_ttf.h>

#include <syslog.h>

struct _app
{
	int quit;
	Leap::Controller controller;

	int screenWidth;
	int screenHeight;
	int fullscreen;

	int windowWidth;
	int windowHeight;

//	SDL_Window *window;
	SDL_Surface *screen;
};
struct _app app;

static input outInput;
input input_get(void)
{
	return outInput;
}

static char *inputPathString;
const char *input_path(void)
{
	return inputPathString;
}
char *input_path_concat(const char *right)
{
	int size = strlen(inputPathString) + strlen(right) + 1;
	char *destination = (char *)calloc(size, sizeof(char));
	strcpy(destination, inputPathString);
	strcat(destination, right);
	return destination;
}
char *concat(const char *left, const char *right)
{
	int size = strlen(left) + strlen(right) + 1;
	char *destination = (char *)calloc(size, sizeof(char));
	strcpy(destination, left);
	strcat(destination, right);
	return destination;
}

struct _mouse
{
	int down;
	int dragX;
	int dragY;
};
struct _mouse mouse;

static void handle_mouse(SDL_Event event)
{
	SDL_MouseButtonEvent button;
	SDL_MouseMotionEvent motion;

	switch(event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
			button = event.button;
			if(!mouse.down) {
				mouse.down = 1;
				mouse.dragX = button.x;
				mouse.dragY = button.y;
			}
		break;
		case SDL_MOUSEBUTTONUP:
			button = event.button;
			mouse.down = 0;
			outInput.angle_x = 0;
			outInput.angle_y = 0;
		break;

		case SDL_MOUSEMOTION:
			motion = event.motion;
			if(mouse.down) {
				outInput.angle_x = -(motion.y - mouse.dragY) * 0.01;
				outInput.angle_y = -(motion.x - mouse.dragX) * 0.01;
			}
		break;
	}
}

static void handle_reset(void)
{
	outInput.velocity = 0.0f;
	outInput.angle_x = 0.0f;
	outInput.angle_y = 0.0f;
	mouse.down = 0;
	mouse.dragX = 0;
	mouse.dragY = 0;
	scene_reset();
}

static void toggle_fullscreen(void)
{
//	SDL_SetWindowFullscreen(app.window, SDL_TRUE);
//	SDL_Delay(100);
//	scene_init();
	
	if(!app.fullscreen) {
		SDL_Surface *surface = SDL_SetVideoMode(app.windowWidth, app.windowHeight, 32,
												SDL_OPENGL | SDL_FULLSCREEN);
		if(surface != NULL) {
			scene_init();
			scene_resize(app.windowWidth, app.windowHeight);
			app.screen = surface;
			app.fullscreen = 1;
		}
	} else {
		SDL_Surface *surface = SDL_SetVideoMode(app.windowWidth, app.windowHeight, 32, SDL_OPENGL);
		if(surface != NULL) {
			scene_init();
			scene_resize(app.windowWidth, app.windowHeight);
			app.screen = surface;
			app.fullscreen = 0;
		}
	}
}

static void handle_keyboard(SDL_KeyboardEvent event)
{
	switch(event.type)
	{
		case SDL_KEYDOWN:
			switch(event.keysym.sym) {
				case SDLK_ESCAPE: app.quit = 1; break;
				case SDLK_r: handle_reset(); break;
				case SDLK_e: toggle_fullscreen(); break;
				case SDLK_t: scene_toggle(); break;

				case SDLK_w:
					outInput.velocity = -1.0f;
				break;
				case SDLK_s:
					outInput.velocity = 1.0f;
				break;
				default: break;
			}
		break;
		case SDL_KEYUP:
			outInput.velocity = 0;
		break;
	}
}

static void handle_leap(void)
{
	if(app.controller.isConnected()) {
		const Leap::Frame frame = app.controller.frame();
		/*std::cout << "Frame id: " << frame.id()
		        << ", timestamp: " << frame.timestamp()
		        << ", hands: " << frame.hands().count()
		        << ", fingers: " << frame.fingers().count()
		        << ", tools: " << frame.tools().count() << std::endl;*/
		printf("hands: %d fingers: %d\n ", frame.hands().count(), frame.fingers().count());

		Leap::Finger throttle;
		Leap::Vector control;
		switch(frame.hands().count()) {
			case 2:
				throttle = (frame.hands()[1]).fingers()[0];
				outInput.velocity = throttle.tipPosition()[2] * 0.01;
			case 1:
				control = ((frame.hands()[0]).fingers()[0]).direction();
				outInput.angle_x = control.pitch() * 0.1;
				outInput.angle_y = -control.yaw() * 0.1;
			break;
			default:
				outInput.velocity = 0;
				outInput.angle_x = 0;
				outInput.angle_y = 0;
			break;
		}
	}
}

static void poll_inputs(void)
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				app.quit = 1;
			break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				handle_keyboard(event.key);
			break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEMOTION:
				handle_mouse(event);
			break;
		}
	}

	handle_leap();
}

int main(int argc, char *argv[])
{
	if(argc == 1) {
		std::string inputPath = std::string(argv[0]) + std::string("/");
		if(inputPath.find("flythrough") != std::string::npos) {
			inputPath = std::string("res/");
		}
		int size = strlen(inputPath.c_str()) + 1;
		inputPathString = (char *)calloc(size, sizeof(char));
		strcpy(inputPathString, inputPath.c_str());
	}

	if(SDL_Init( SDL_INIT_EVERYTHING ) < 0)
		return 1;
//	if (TTF_Init() != 0)
//	{
//		SDL_Quit();
//		return 1;
//	}

	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	app.screenWidth = info->current_w;
	app.screenHeight = info->current_h;
	app.windowWidth = (int)(info->current_w  * 0.8);
	app.windowHeight = (int)(info->current_h * 0.8);
	printf("Screen Width[%d] Height[%d]\n", app.screenWidth, app.screenHeight);
	printf("Window Width[%d] Height[%d]\n", app.windowWidth, app.windowHeight);
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
//	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

	app.screen = SDL_SetVideoMode(app.windowWidth, app.windowHeight, 32, SDL_OPENGL);
	if(app.screen == NULL)
		return 1;
	app.fullscreen = 0;

	SDL_EnableUNICODE(SDL_TRUE);
	SDL_WM_SetCaption("Interactive Flythrough of Universe", NULL);

/*
	app.windowWidth = 1280;
	app.windowHeight = 800;
	SDL_Window *window = SDL_CreateWindow("Interactive Flythrough of Universe",
										  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										  app.windowWidth, app.windowHeight,
										  SDL_WINDOW_OPENGL);
	 SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
	 														 SDL_RENDERER_TARGETTEXTURE);
	if(window == NULL) {
		SDL_Quit();
		return 1;
	}
	app.window = window;
*/

	scene_load();
	scene_init();
	scene_resize(app.windowWidth, app.windowHeight);

	app.quit = 0;
	int last = 0;
	while(!app.quit)
	{
		int elaps = SDL_GetTicks() - last;
		float delta = elaps / 1000;

        /* Handle Inputs */
		poll_inputs();

        /* Update Scene */
        scene_update(delta);

        /* Render Scene */
        scene_render(delta);

        last = SDL_GetTicks();
//		SDL_RenderPresent(renderer);
        SDL_GL_SwapBuffers();
		SDL_Delay(33);
	}

	/* Clean up */
	SDL_Quit();

	return 0;
}
