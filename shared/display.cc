#include "display.h"
#include "gui.h"
#include <SDL2/SDL_opengl.h>
#include <stdio.h>

/* Global display instance */
struct display display;

void display_destroy(void){
    /* clean-up */
    if (display.back_buffer) {
        for (int i = 0; i < WIDTH; i++) {
            free(display.back_buffer[i]);
        }
        free(display.back_buffer);
    }
    gui_cleanup();
    SDL_GL_DeleteContext(display.glcontext);
    SDL_DestroyWindow(display.window);
}


int display_initialize(
    bool fullscreen,
    int *steps,
    bool *paused,
    bool *load_store_quirk,
    bool *shift_quirk,
    bool *vwrap,
    bool *muted
) {

    display.WINDOW_WIDTH = WIDTH * (int)SCALE;
    display.WINDOW_HEIGHT = HEIGHT * (int)SCALE;
    display.back_buffer = NULL;
    display.window = NULL;
    display.fullscreen_flag = 0;
    display.vsync_flag = 0;
    display.limit_fps_flag = 1;
    display.lost_window_focus = 0;

    /* set rendering colors */
    display.background_color[0] = (float) DEFAULT_BACKGROUND_R / (float) 0xFF;
    display.background_color[1] = (float) DEFAULT_BACKGROUND_G / (float) 0xFF;
    display.background_color[2] = (float) DEFAULT_BACKGROUND_B / (float) 0xFF;

    display.foreground_color[0] = (float) DEFAULT_FOREGROUND_R / (float) 0xFF;
    display.foreground_color[1] = (float) DEFAULT_FOREGROUND_G / (float) 0xFF;
    display.foreground_color[2] = (float) DEFAULT_FOREGROUND_B / (float) 0xFF;

    /* initialize gui state */
    gui_create();

    int window_mode = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    /* init the backbuffer */
    display.back_buffer = (unsigned char **) malloc(WIDTH * sizeof(unsigned char *));
    const char *err_str = "Unable to allocate memory on the heap.\n";

    if(!display.back_buffer) {
        fprintf(stderr, "%s", err_str);
        return 1;
    }
    memset(display.back_buffer, 0, WIDTH * sizeof(unsigned char *));

    for (int i = 0; i < WIDTH; i++) {
        display.back_buffer[i] = (unsigned char *) malloc(HEIGHT * sizeof(unsigned char));
        if(!display.back_buffer[i]) {
            fprintf(stderr, "%s", err_str);
            return 1;
        }
        memset(display.back_buffer[i], 0, HEIGHT * sizeof(unsigned char));
    }

    /* setup window with openGL context */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    display.window = SDL_CreateWindow(
        "Kiwi8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        display.WINDOW_WIDTH,
        display.WINDOW_HEIGHT,
        window_mode
    );

    if (display.window == NULL) {
        fprintf(stderr, "Error: %s\n", SDL_GetError());
        return 1;
    }

    display.glcontext = SDL_GL_CreateContext(display.window);

    /* disable V-Sync */
    SDL_GL_SetSwapInterval(0);

    /* specify the texture */
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        WIDTH,
        HEIGHT,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        (GLvoid *) display.texture
    );

    /* configure the texture */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /* enable textures */
    glEnable(GL_TEXTURE_2D);

    /* setup ImGui binding */
    gui_initialize(
        &display,
        steps,
        paused,
        load_store_quirk,
        shift_quirk,
        vwrap,
        muted
    );

    /* set to fullscreen mode if flag present */
    if (fullscreen) display_toggle_fullscreen();

    return 0;
}

void display_resize(int x, int y) {
    /* get the current window size */
    display.WINDOW_WIDTH = x;
    display.WINDOW_HEIGHT = y;
}

void display_toggle_fullscreen(void) {
    /* check if already fullscreen */
    if (SDL_GetWindowFlags(display.window) & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        /* set windowed */
        SDL_SetWindowFullscreen(display.window, 0);
        SDL_ShowCursor(SDL_ENABLE);
        display.fullscreen_flag = 0;

    } else {
        /* set fullscreen */
        SDL_SetWindowFullscreen(display.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        /* currently, a new ImGui Frame will draw the mouse cursor
           regardless of SDL2's cursor visibility function */
        SDL_ShowCursor(SDL_DISABLE);
        display.fullscreen_flag = 1;

    }
}

void display_toggle_vsync(void) {
    if (SDL_GL_GetSwapInterval()) {
        SDL_GL_SetSwapInterval(0);
        display.vsync_flag = 0;
    } else {
        SDL_GL_SetSwapInterval(1);
        display.vsync_flag = 1;
    }
}

void display_raise_window(void) {
    SDL_RaiseWindow(display.window);
    display.lost_window_focus = 0;
}

void display_render_frame(unsigned char **frame){
    gui_new_frame();

    /* copy the frame to back_buffer */
    if (frame != NULL) {
        for (int i = 0; i < WIDTH; i++) {
            memcpy(display.back_buffer[i], frame[i], HEIGHT * sizeof(unsigned char));
        }
    }

    /* set Viewport & Clear the screen (sets the background color) */
    glViewport(0, 0, display.WINDOW_WIDTH, display.WINDOW_HEIGHT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < WIDTH; i++){
        for (int j = 0; j < HEIGHT; j++){
            if (display.back_buffer[i][HEIGHT-j-1]) {
                /* Fill the foreground pixel */
                display.texture[j][i][0] = (unsigned char)(display.foreground_color[0] * (float) 0xFF); //R
                display.texture[j][i][1] = (unsigned char)(display.foreground_color[1] * (float) 0xFF); //G
                display.texture[j][i][2] = (unsigned char)(display.foreground_color[2] * (float) 0xFF); //B

            } else {
                /* Fill the background pixel */
                display.texture[j][i][0] = (unsigned char)(display.background_color[0] * (float) 0xFF); //R
                display.texture[j][i][1] = (unsigned char)(display.background_color[1] * (float) 0xFF); //G
                display.texture[j][i][2] = (unsigned char)(display.background_color[2] * (float) 0xFF); //B
            }
        }
    }

    /* send texture to GPU */
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        WIDTH,
        HEIGHT,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        (GLvoid *) display.texture
    );

    /* create room at the top for menu bar */
    float top_edge = gui.show_menu_flag ?
        (float)(display.WINDOW_HEIGHT - MENU_HEIGHT) / display.WINDOW_HEIGHT : (float) 1.0;

    /* render the texture */
    glBegin(GL_QUADS);

        /* bottom left */
        glTexCoord2f(0.0, 0.0);
        glVertex2f(-1.0, -1.0);

        /* bottom right */
        glTexCoord2f(1.0, 0.0);
        glVertex2f(1.0, -1.0);

        /* top right */
        glTexCoord2f(1.0, 1.0);
        glVertex2f(1.0, top_edge);

        /* top left */
        glTexCoord2f(0.0, 1.0);
        glVertex2f(-1.0, top_edge);

    glEnd();

    gui_render();
    SDL_GL_SwapWindow(display.window);
}
