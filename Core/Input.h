#ifndef INPUT_H
#define INPUT_H

#include "Renderer.h"

#include <SDL2/SDL.h>

class Input {
	private:
		SDL_Event event;
		const Uint8 *state;

		void CheckKeys(unsigned char *keys);
		int CheckOS(Renderer *renderer);

	public:
		Input();
		~Input();

		int Poll(Renderer *renderer, unsigned char *keys, SDL_mutex *data_lock);
		
};

#endif
