#pragma once
#include "interface.hpp"
#include "config.hpp"
#include <cstring>

const float SCORE_WIDTH  = (MAP_BLOCK_SIZE*4.8);
const float SCORE_HEIGHT = (MAP_BLOCK_SIZE*2);

const int COUNTDOWN_DELAY = 800;

struct Score{
    int left=0,right=0;
    SDL_Texture* text=NULL;
    TTF_Font * score_font;
    SDL_Color color = {200,200,100};
    SDL_Color color_countdown = {250,100,100};
    SDL_Rect dest;
    //count down
    unsigned long long ms = 0;
    char countdown_text[3][10] = {"Ready","Set","Go!"};
    int step = 0;
    void generate(Interface& screen){
        char char_text[100];
        sprintf(char_text,"%2d : %2d",left,right);
        SDL_Surface* surface_text = TTF_RenderText_Solid(score_font, char_text, color);
        if (text!=NULL)
            SDL_DestroyTexture(text);
        text = SDL_CreateTextureFromSurface(screen.renderer, surface_text);
        SDL_QueryTexture(text, NULL, NULL, &dest.w, &dest.h);
        dest.x = SCREEN_CENTERX-(dest.w/2), dest.y = SCREEN_CENTERY-(dest.h/2)-MAP_BLOCK_SIZE/2;
        SDL_FreeSurface(surface_text);
    }
    Score(Interface &screen, const char *front){        
        score_font = TTF_OpenFont(front, 255);
        generate(screen);
    }
    bool getCountdown(Interface& screen,unsigned long long &now){
        if (step >0 && now - ms < COUNTDOWN_DELAY) return false;
        if (step>=3) return true;
        SDL_Surface* surface_text = TTF_RenderText_Solid(score_font, countdown_text[step++], color_countdown);
        if (text!=NULL)
            SDL_DestroyTexture(text);
        text = SDL_CreateTextureFromSurface(screen.renderer, surface_text);
        SDL_QueryTexture(text, NULL, NULL, &dest.w, &dest.h);
        dest.x = SCREEN_CENTERX-(dest.w/2), dest.y = SCREEN_CENTERY-(dest.h/2)-MAP_BLOCK_SIZE/2;
        SDL_FreeSurface(surface_text);
        ms = now;
        return false;
    }
    void draw(Interface& screen){
        SDL_RenderCopy(screen.renderer, text, NULL, &dest);
    }
    
};