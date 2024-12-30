#ifndef __INTERFACE_HEADER_
#define __INTERFACE_HEADER_
#include "config.hpp"

struct Interface {
    SDL_Window * window = NULL;
    SDL_Surface * surface = NULL;
    SDL_Renderer * renderer = NULL;
    SDL_Texture * texture = NULL;
    SDL_Rect SCREEN_DEST;
    unsigned char bgRGBA[4] = {30,30,30,255};
    Interface() {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        window = SDL_CreateWindow("display", 0, 0, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
        surface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SCREEN_DEST.x=SCREEN_DEST.y=0;
        SCREEN_DEST.w=LOGICAL_SCREEN_WIDTH,SCREEN_DEST.h=LOGICAL_SCREEN_HEIGHT;
        SDL_RenderSetLogicalSize(renderer, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT);
        if (TTF_Init() < 0)
        {
            printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
            exit(1);
        }
    }
    void setRGBA(){
        SDL_SetRenderDrawColor(renderer, bgRGBA[0], bgRGBA[1], bgRGBA[2], bgRGBA[3]);
    }
    void setRGBA(char r,char g,char b,char a){
        bgRGBA[0]=r,
        bgRGBA[1]=g,
        bgRGBA[2]=b,
        bgRGBA[3]=a;
        setRGBA();
    }
    void drawBackground(){
        setRGBA();
        SDL_RenderClear(renderer);
    }
    void present(){
        SDL_RenderPresent(renderer);
        SDL_Delay(0);
    }
    void close() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(texture);
        TTF_Quit();
        SDL_Quit();
    }
};

#endif