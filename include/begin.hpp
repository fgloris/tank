#pragma once
#include "interface.hpp"
#include "config.hpp"

struct Begin{
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    SDL_Rect dest;
    double rgb[3];

    Begin(SDL_Renderer * rd,const char * name,double r,double g,double b){
        renderer = rd,
        texture = IMG_LoadTexture(rd, name);
        if (texture == NULL){
            std::cout<<"load beginning video texture failed!\n";
            exit(1);
        }
        rgb[0]=r,rgb[1]=g,rgb[2]=b;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        while (dest.w>LOGICAL_SCREEN_WIDTH/2 || dest.h>LOGICAL_SCREEN_HEIGHT/2){
            dest.w/=1.5,dest.h/=1.5;
        }
        dest.x = (LOGICAL_SCREEN_WIDTH-dest.w)/2;
        dest.y = (LOGICAL_SCREEN_HEIGHT-dest.h)/2;
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    };
    void draw(){
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }
    void destroy(){
        SDL_DestroyTexture(texture);
    }
    void display(){
        for (double alpha=0.3;alpha<=1;alpha+=0.004){
            SDL_SetRenderDrawColor(renderer, rgb[0]*alpha, rgb[1]*alpha, rgb[2]*alpha, 255);
            SDL_RenderClear(renderer);
            draw();
            SDL_RenderPresent(renderer);
            SDL_Delay(0);
        }
        SDL_Delay(1500);
        for (double alpha=1;alpha>0;alpha-=0.002){
            SDL_SetRenderDrawColor(renderer, rgb[0]*alpha, rgb[1]*alpha, rgb[2]*alpha, 255);
            SDL_RenderClear(renderer);
            draw();
            SDL_RenderPresent(renderer);
            SDL_Delay(0);
        }
        destroy();
    }
};