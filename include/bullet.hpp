#ifndef __BULLET_HEADER_
#define __BULLET_HEADER_
#include "config.hpp"
#include "map.hpp"

const int BULLET_RADIUS = 5;
#define BULLET_SPEED (TANK_DEFAULT_MOVE_SPEED*2.0)
#define BULLET_LIFE 10000

#define BULLET_COLLIDE_NONE 0
#define BULLET_COLLIDE_X 1
#define BULLET_COLLIDE_Y 2

#define BULLET_TYPE_DEFAULT 0
#define BULLET_TYPE_PIERCE 1
#define BULLET_GATLING_LIFE 8

#define BULLET_PIERCE_LENGTH 20
#define BULLET_PIERCE_SPEED 2
struct Bullet{
    Point2f pos;
    float angle;
    unsigned long long birth;
    Bullet(unsigned long long &now,Point2f _pos,float _angle):pos(_pos),birth(now),angle(_angle){
    };
    bool checkAlive(unsigned long long &now){
        if (now-birth>BULLET_LIFE) return false;
        return true;
    }
    int collideWithBlock(SDL_Rect &rect,float prev_x,float prev_y){
        if ((pos.x>float(rect.x))&&(pos.x<float(rect.x+rect.w))&&(pos.y>float(rect.y))&&(pos.y<float(rect.y+rect.h))){
            if ((prev_x<=float(rect.x)+0.01) || (prev_x>=float(rect.x+rect.w)-0.01)){
                return BULLET_COLLIDE_X;
            }
            else if (prev_y<=float(rect.y+0.01) || prev_y>=float(rect.y+rect.h-0.01)){
                return BULLET_COLLIDE_Y;                
            }
        }
        return BULLET_COLLIDE_NONE;
    }
    void update(Map &map){
        int x_block = int(pos.x/MAP_BLOCK_SIZE);
        int y_block = int(pos.y/MAP_BLOCK_SIZE);

        float y = -BULLET_SPEED*std::sin(angle),
              x = BULLET_SPEED*std::cos(angle);

        pos.x+=x;
        pos.y+=y;
        if (pos.x<BLOCK_THICK || pos.x>LOGICAL_SCREEN_WIDTH-BLOCK_THICK){
            pos.x-=x;
            angle = PI-angle;
            return;
        }
        else if (pos.y<BLOCK_THICK || pos.y>LOGICAL_SCREEN_HEIGHT-BLOCK_THICK){
            pos.y-=y;
            angle = -angle;
            return;
        }
        int res;

        int x_block_precice = int((pos.x*2)/MAP_BLOCK_SIZE)%2;
        int y_block_precice = int((pos.y*2)/MAP_BLOCK_SIZE)%2;

        for (int yi=std::max(y_block+y_block_precice-1,0);yi<=std::min(y_block+y_block_precice,MAP_BLOCK_ROWS);yi++){
            for (int xi=std::max(x_block+x_block_precice-1,0);xi<=std::min(x_block+x_block_precice,MAP_BLOCK_COLS);xi++){
                if (map.data[yi][xi] == BLOCK_HORIZ || map.data[yi][xi] == BLOCK_VERTI){
                    res = collideWithBlock(map.collide_rects[yi][xi],pos.x-x,pos.y-y);
                    if (res == BULLET_COLLIDE_X){
                        pos.x-=x;
                        angle = PI-angle;
                        return;
                    }else if (res == BULLET_COLLIDE_Y){
                        pos.y-=y;
                        angle = -angle;
                        return;
                    }
                }
            }
        }
    }
};
struct GatlingBullet{
    Point2f pos;
    float angle;
    int life = BULLET_GATLING_LIFE;
    unsigned char r,g,b;
    GatlingBullet(Point2f p,float dir,unsigned char _r,unsigned char _g,unsigned char _b):pos(p),angle(dir),r(_r),g(_g),b(_b){
    };
    int collideWithBlock(SDL_Rect &rect,float prev_x,float prev_y){
        if ((pos.x>float(rect.x))&&(pos.x<float(rect.x+rect.w))&&(pos.y>float(rect.y))&&(pos.y<float(rect.y+rect.h))){
            if ((prev_x<=float(rect.x)) || (prev_x>=float(rect.x+rect.w))){
                return BULLET_COLLIDE_X;
            }
            else if (prev_y<=float(rect.y) || prev_y>=float(rect.y+rect.h)){
                return BULLET_COLLIDE_Y;                
            }
        }
        return BULLET_COLLIDE_NONE;
    }
    void update(Map &map){
        if (life<=0) return;
        int x_block = int(pos.x/MAP_BLOCK_SIZE);
        int y_block = int(pos.y/MAP_BLOCK_SIZE);

        float y = -BULLET_SPEED*std::sin(angle),
              x = BULLET_SPEED*std::cos(angle);

        pos.x+=x;
        pos.y+=y;
        if (pos.x<BLOCK_THICK || pos.x>LOGICAL_SCREEN_WIDTH-BLOCK_THICK){
            pos.x-=x;
            angle = PI-angle;
            life--;
            return;
        }
        else if (pos.y<BLOCK_THICK || pos.y>LOGICAL_SCREEN_HEIGHT-BLOCK_THICK){
            pos.y-=y;
            angle = -angle;
            life--;
            return;
        }
        int res;

        int x_block_precice = int((pos.x*2)/MAP_BLOCK_SIZE)%2;
        int y_block_precice = int((pos.y*2)/MAP_BLOCK_SIZE)%2;

        for (int yi=std::max(y_block+y_block_precice-1,0);yi<=std::min(y_block+y_block_precice,MAP_BLOCK_ROWS);yi++){
            for (int xi=std::max(x_block+x_block_precice-1,0);xi<=std::min(x_block+x_block_precice,MAP_BLOCK_COLS);xi++){
                if (map.data[yi][xi] == BLOCK_HORIZ || map.data[yi][xi] == BLOCK_VERTI){
                    res = collideWithBlock(map.collide_rects[yi][xi],pos.x-x,pos.y-y);
                    if (res == BULLET_COLLIDE_X){
                        pos.x-=x;
                        angle = PI-angle;
                        life--;
                        return;
                    }else if (res == BULLET_COLLIDE_Y){
                        pos.y-=y;
                        angle = -angle;
                        life--;
                        return;
                    }
                }
            }
        }
    }
};
struct PierceBullet{
    Point2f pos1,pos2;
    float angle;
    bool alive = true;
    PierceBullet(Point2f p,float dir):pos1(p),angle(dir),pos2(0,0){
        float y = -BULLET_PIERCE_LENGTH*std::sin(angle),
              x = BULLET_PIERCE_LENGTH*std::cos(angle);
        pos2.x = pos1.x+x;
        pos2.y = pos1.y+y;
    };
    void update(){
        float y = -BULLET_PIERCE_SPEED*std::sin(angle),
              x = BULLET_PIERCE_SPEED*std::cos(angle);

        pos1.x+=x;
        pos1.y+=y;
        pos2.x+=x;
        pos2.y+=y;

        if (pos2.x<BLOCK_THICK || pos2.x>LOGICAL_SCREEN_WIDTH-BLOCK_THICK ||
            pos2.y<BLOCK_THICK || pos2.y>LOGICAL_SCREEN_HEIGHT-BLOCK_THICK
        ){
            alive = false;
        }
    }
};
#endif