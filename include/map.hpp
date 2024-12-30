#ifndef __MAP_HEADER_
#define __MAP_HEADER_
#include "interface.hpp"
#include "config.hpp"
#include <chrono>

struct Map{
    char data[MAP_BLOCK_ROWS][MAP_BLOCK_COLS];
    SDL_Rect rects[MAP_BLOCK_ROWS*MAP_BLOCK_COLS+4];
    SDL_Rect collide_rects[MAP_BLOCK_ROWS][MAP_BLOCK_COLS];
    int rects_size=0;
    void translate(){
        rects_size=0;
        for (int y=1;y<=MAP_BLOCK_ROWS;y++){
            for (int x=1;x<=MAP_BLOCK_COLS;x++){
                if (data[y-1][x-1] == BLOCK_HORIZ){
                    SDL_Rect block_rect{MAP_BLOCK_SIZE*(x-1),MAP_BLOCK_SIZE*y,BLOCK_LENGTH,BLOCK_THICK};
                    rects[rects_size++] = block_rect;
                    collide_rects[y-1][x-1] = block_rect;
                }else if (data[y-1][x-1] == BLOCK_VERTI){
                    SDL_Rect block_rect{MAP_BLOCK_SIZE*x,MAP_BLOCK_SIZE*(y-1),BLOCK_THICK,BLOCK_LENGTH};
                    rects[rects_size++] = block_rect;
                    collide_rects[y-1][x-1] = block_rect;
                }
            }
        }
        SDL_Rect block_rect_l{0,0,BLOCK_THICK,LOGICAL_SCREEN_HEIGHT};
        rects[rects_size++] = block_rect_l;
        SDL_Rect block_rect_t{0,0,LOGICAL_SCREEN_WIDTH,BLOCK_THICK};
        rects[rects_size++] = block_rect_t;
        SDL_Rect block_rect_r{LOGICAL_SCREEN_WIDTH-BLOCK_THICK,0,BLOCK_THICK,LOGICAL_SCREEN_HEIGHT};
        rects[rects_size++] = block_rect_r;
        SDL_Rect block_rect_b{0,LOGICAL_SCREEN_HEIGHT-BLOCK_THICK,LOGICAL_SCREEN_WIDTH,BLOCK_THICK};
        rects[rects_size++] = block_rect_b;
    }
    void getConnected(){
        char reigon[MAP_BLOCK_ROWS][MAP_BLOCK_COLS]={0};
        reigon[0][0]=1;
        int connect = 1;
        next:
        bool opera=true;
        while (opera){
            opera = false;
            for (int y=0;y<MAP_BLOCK_ROWS;y++){
                for (int x=0;x<MAP_BLOCK_COLS;x++) if (!reigon[y][x]){
                    if ((y>0 && data[y-1][x]!=BLOCK_HORIZ && reigon[y-1][x])
                     || (x>0 && data[y][x-1]!=BLOCK_VERTI && reigon[y][x-1])
                     || (y<MAP_BLOCK_ROWS-1 && data[y][x]!=BLOCK_HORIZ && reigon[y+1][x])
                     || (x<MAP_BLOCK_COLS-1 && data[y][x]!=BLOCK_VERTI && reigon[y][x+1])){
                        reigon[y][x]=1;
                        connect++;
                        opera = true;
                    }
                }
            }
        }
        if (connect<MAP_BLOCK_ROWS*MAP_BLOCK_COLS){
            for (int y=0;y<MAP_BLOCK_ROWS;y++){
                for (int x=0;x<MAP_BLOCK_COLS;x++) if (reigon[y][x]){
                    if (y<MAP_BLOCK_ROWS-1 && data[y][x]==BLOCK_HORIZ && !reigon[y+1][x]){
                        data[y][x]=BLOCK_EMPTY;
                        reigon[y+1][x]=1;
                        connect++;
                        goto next;
                    }
                    else if (x<MAP_BLOCK_COLS-1 && data[y][x]==BLOCK_VERTI && !reigon[y][x+1]){
                        data[y][x]=BLOCK_EMPTY;
                        reigon[y][x+1]=1;
                        connect++;
                        goto next;
                    }
                }
            }
        }

    }
    void generate(){
        uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        srand(ms);
        for (int i=0;i<MAP_BLOCK_ROWS-1;i++){
            for (int j=0;j<MAP_BLOCK_COLS-1;j++){
                data[i][j]=rand()%3;
            }
        }
        bool is_in_turnnel = false; int turnnel_start = 0;
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        for (int i=0;i<MAP_BLOCK_ROWS;i++){
            if (i<MAP_BLOCK_COLS-1 && data[i][MAP_BLOCK_COLS-2]==BLOCK_EMPTY && data[i+1][MAP_BLOCK_COLS-2]!=BLOCK_VERTI)
                data[i][MAP_BLOCK_COLS-1]=BLOCK_HORIZ;
            else
                data[i][MAP_BLOCK_COLS-1]=BLOCK_EMPTY;
            if (data[i][MAP_BLOCK_COLS-2] == BLOCK_VERTI && !is_in_turnnel){
                is_in_turnnel = true;
                turnnel_start = i;
            }
            else if (data[i][MAP_BLOCK_COLS-2] != BLOCK_VERTI && is_in_turnnel){
                is_in_turnnel = false;
                data[positive(turnnel_start-1)+rand()%(i-turnnel_start+1)][MAP_BLOCK_COLS-1]=BLOCK_HORIZ;
            }

        }
        is_in_turnnel = false;
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        for (int j=0;j<MAP_BLOCK_COLS;j++){
            if (j<MAP_BLOCK_COLS-1 && data[MAP_BLOCK_ROWS-2][j]==BLOCK_EMPTY && data[MAP_BLOCK_ROWS-2][j+1]!=BLOCK_HORIZ)
                data[MAP_BLOCK_ROWS-1][j]=BLOCK_VERTI;
            else
                data[MAP_BLOCK_ROWS-1][j]=BLOCK_EMPTY;
            if (data[MAP_BLOCK_ROWS-2][j] == BLOCK_HORIZ && !is_in_turnnel){
                is_in_turnnel = true;
                turnnel_start = j;
            }
            else if (data[MAP_BLOCK_ROWS-2][j] != BLOCK_HORIZ && is_in_turnnel){
                is_in_turnnel = false;
                data[MAP_BLOCK_ROWS-1][positive(turnnel_start-1)+rand()%(j-turnnel_start+1)]=BLOCK_VERTI;
            }
        }
        getConnected();
        translate();
    }
    Map(){
        generate();
    }
    void draw(Interface &screen){
        SDL_SetRenderDrawColor(screen.renderer,250,250,250,255);
        SDL_RenderFillRects(screen.renderer,rects,rects_size);
    }
};
#endif