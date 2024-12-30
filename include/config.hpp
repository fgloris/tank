#ifndef __CONFIG_HEADER_
#define __CONFIG_HEADER_
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <time.h> 
#include <cstdlib>
#include <bits/stdc++.h>

//screen
#define SCREEN_WIDTH 2560
#define SCREEN_HEIGHT 1600
#define LOGICAL_SCREEN_WIDTH (SCREEN_WIDTH)
#define LOGICAL_SCREEN_HEIGHT (SCREEN_HEIGHT)
#define SCREEN_CENTERX (LOGICAL_SCREEN_WIDTH/2)
#define SCREEN_CENTERY (LOGICAL_SCREEN_HEIGHT/2)

#define FRAME_RATE 600
const unsigned long long FRAME_LATENCY = 1/FRAME_RATE;

//map
#define MAP_BLOCK_SIZE 160 //should be a factor of SCREEN_HEIGHT
const int MAP_BLOCK_COLS = (LOGICAL_SCREEN_WIDTH/MAP_BLOCK_SIZE);
const int MAP_BLOCK_ROWS = (LOGICAL_SCREEN_HEIGHT/MAP_BLOCK_SIZE);

const int BLOCK_THICK = (MAP_BLOCK_SIZE/10);
const int BLOCK_LENGTH = MAP_BLOCK_SIZE+(BLOCK_THICK/2);
enum BlockType{
    BLOCK_EMPTY=0,
    BLOCK_HORIZ=1,
    BLOCK_VERTI=2
};

//point
const float PI = 3.1416;
struct Point2f{
    float x,y;
    Point2f(){x=y=0;}
    Point2f(float _x, float _y){
        x=_x;
        y=_y;
    }
    Point2f(SDL_Point &p){
        x=p.x;
        y=p.y;
    }
    void toPolar(){
        float r = sqrt(x*x+y*y);
        float a = atan(y/x);
        if (x<0) a+=PI;
        x = r;
        y = a;
    }
    void toUnit(){
        float r = sqrt(x*x+y*y);
        x/=r,y/=r;
    }
    SDL_Point toCartesian(float rotate,Point2f &center){
        return SDL_Point{int(x*sin(y+rotate)+center.x),int(x*cos(y+rotate)+center.y)};
    }
    Point2f operator+(const Point2f &other){
        return Point2f(x+other.x,y+other.y);
    }
    void operator+=(const Point2f &other){
        x+=other.x,y+=other.y;
    }
    void operator-=(const Point2f &other){
        x-=other.x,y-=other.y;
    }
    Point2f operator-(const Point2f &other){
        return Point2f(x-other.x,y-other.y);
    }
    float operator^(const Point2f &other){
        return sqrt((x-other.x)*(x-other.x)+(y-other.y)*(y-other.y));
    }
    void operator*=(const float &other){
        x*=other,y*=other;
    }
    float operator*(const Point2f &other){
        return x*other.y-y*other.x;
    }
    bool operator<(const Point2f &other){
        return (x<other.x)&&(y<other.y);
    }
};

//func
#define abs(x) (((x)>0)?(x):(-(x)))
#define positive(x) (((x)>0)?(x):0)
#define random(x) ((rand()%(x))-((x)/2))

void randomTwoPoints(Point2f &A,Point2f &B);
bool lineWithLine(Point2f p1,Point2f p2,Point2f &p3,Point2f &p4);

Uint32 toGfxColor(unsigned char r,unsigned char g,unsigned char b,unsigned char a);

Uint32 addAlpha(const Uint32 &color, unsigned char a);

//tank
const float TANK_DEFAULT_MOVE_SPEED = 0.3;
const float TANK_PRECISE_ROTATE_SPEED = 0.002;
const float TANK_DEFAULT_ROTATE_SPEED = 0.006;

const float TANK_BODY_WIDTH  = 50;
const float TANK_BODY_HEIGHT = 60;
const float TANK_CANNON_LENGTH = 50;
const float TANK_CANNON_THICK = 14;
const float TANK_LINE_THICK = 3;
const float TANK_CANNON_RADIUS = (TANK_BODY_WIDTH/2);
const float CONTROL_RADIUS_PRECISE = TANK_CANNON_RADIUS*1.5;

const float TANK_FIRE_COOLDOWN = 250;
const float TANK_GATLING_FIRE_COOLDOWN = 20;
const float TANK_PIERCE_FIRE_COOLDOWN = 850;

//bullet

//color
enum ColorSystem{
    RED=0,BLUE=1
};

const Uint32 TANK_COLOR[2] = {toGfxColor(255,10,10,255),toGfxColor(10,10,255,255)};
const Uint32 SHELL_COLOR[2] = {toGfxColor(255,200,80,0),toGfxColor(80,200,255,0)};
const Uint32 COOLDOWN_COLOR[2] = {toGfxColor(255,200,80,100),toGfxColor(80,200,255,100)};
const SDL_Color GATLING_COLOR[2] = {SDL_Color{205,80,30},SDL_Color{30,80,205}};

//control
enum ControlSystem{
    LEFT_KEYBOARD,RIGHT_KEYBOARD,MOUSE,ONLINE,AI
};

//special
const float SPECIAL_CHAIN_LENGTH = 20;
const float SPECIAL_CHAIN_WIDTH = 4;
const float SPECIAL_CHAIN_NODE_LENGTH = (0.75*SPECIAL_CHAIN_LENGTH);
const float SPECIAL_CHAIN_THICK = 6;
const float SPECIAL_CHAIN_NODE_STEP1 = (0.5*SPECIAL_CHAIN_LENGTH);
const float SPECIAL_CHAIN_NODE_STEP2 = (1.25*SPECIAL_CHAIN_LENGTH);

const Uint32 COLOR_WHITE = toGfxColor(255,255,255,255);
const Uint32 SPECIAL_CHAIN_COLOR[2] = {toGfxColor(100,100,200,255),toGfxColor(200,100,100,255)};
#endif