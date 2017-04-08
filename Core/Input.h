#ifndef INPUT_H
#define INPUT_H

#include "Display.h"
#include <SDL2/SDL.h>

#define NUM_KEYS 16
#define USER_QUIT 1
#define SOFT_RESET -1
#define SIGNAL_DRAW 9999

class Input {
	private:
		/* For processing window/keyboard events */
		SDL_Event event;
		const Uint8 *state;

		Display *display;
		SDL_mutex *data_lock;

		int CheckEvents();
		void CheckKeys();

	public:
		Input();
		~Input();

		/* HEX based keypad (0x0-0xF) */
		unsigned char keys[NUM_KEYS];

		void Initialize(Display *display, SDL_mutex *data_lock);
		void Reset();
		int Poll();
		
};

#endif
