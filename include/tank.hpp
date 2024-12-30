#pragma once
#include "config.hpp"
#include "score.hpp"
#include "buff.hpp"
#include "bullet.hpp"
#include "map.hpp"
#include "special.hpp"
#include "bullet_updater.hpp"
/*
    0------3
    |      |
    |      |
    |      |
    |      |
    1------2
        |
        |
        8

*/

struct Tank{
    float angle = 0;
    Map *map;
    Bullets *bullets;
    SpecialEffect *special_effect;
    ColorSystem color;
    ControlSystem control;
    Point2f center = {0,0};
    Point2f vertices[10] = {
        Point2f(-(TANK_BODY_WIDTH/2),-(TANK_BODY_HEIGHT/2)),Point2f(-(TANK_BODY_WIDTH/2),(TANK_BODY_HEIGHT/2)),Point2f((TANK_BODY_WIDTH/2),(TANK_BODY_HEIGHT/2)),Point2f((TANK_BODY_WIDTH/2),-(TANK_BODY_HEIGHT/2)),
        Point2f(-(TANK_CANNON_THICK)/2,(TANK_CANNON_LENGTH)),Point2f(-(TANK_CANNON_THICK)/2,0.),Point2f((TANK_CANNON_THICK)/2,0.),Point2f((TANK_CANNON_THICK)/2,(TANK_CANNON_LENGTH)),
        Point2f(0.,TANK_CANNON_LENGTH),Point2f(0.,(TANK_CANNON_RADIUS+TANK_CANNON_LENGTH)/2)};
    SDL_Point points[10];
    BuffParams buff;
    unsigned long long now = 0;
    void getPoints(){
        for (int i=0;i<10;i++){
            points[i] = vertices[i].toCartesian(angle,center);
        }
    }
    Tank(float _angle, Point2f _center, ColorSystem _color, ControlSystem _control, Map* _map, Bullets *_bullets, SpecialEffect *_special_effect){
        map = _map;
        bullets = _bullets;
        angle = _angle;
        center = _center;
        color = _color;
        control = _control;
        special_effect = _special_effect;
        for (Point2f &p:vertices)
            p.toPolar();
        getPoints();
        buff = BuffParams();
        now = 0;
    }
    void getTime(unsigned long long &_now){now = _now;}
    void fire(){
        if (buff.aggressive_buff != BUFF_NONE) return;
        if (now-buff.aggressive_cooldown<BUFF_COOLDOWNS[BUFF_NONE]) return;
        buff.aggressive_cooldown = now;
        bullets->que.push_back(Bullet(now,points[9],angle));
    }
    void fireGatling(){
        if (buff.aggressive_buff != BUFF_GATLING) return;
        if (now-buff.aggressive_cooldown<BUFF_COOLDOWNS[BUFF_GATLING]) return;
        buff.aggressive_cooldown = now;
        float r = (static_cast <float> (rand()-RAND_MAX/2) / static_cast <float> (RAND_MAX))*0.8;
        bullets->gatling_que.push_back(GatlingBullet(points[9],angle+r,GATLING_COLOR[color].r+(rand()%100-50),GATLING_COLOR[color].g+(rand()%100-50),GATLING_COLOR[color].b+(rand()%100-50)));
    }
    void firePierce(){
        if (buff.aggressive_buff != BUFF_PIERCE) return;
        if (now-buff.aggressive_cooldown<BUFF_COOLDOWNS[BUFF_PIERCE]) return;
        buff.aggressive_cooldown = now;
        bullets->pierce_que.push_back(PierceBullet(points[9],angle));
    }
    void PointsMoveX(int x){
        for (int i=0;i<10;i++){
            points[i].x+=x;
        }
    }
    void PointsMoveY(int y){
        for (int i=0;i<10;i++){
            points[i].y+=y;
        }
    }
    bool outOfMap(){
        for (int i=0;i<4;i++){
            if ((points[i].x>LOGICAL_SCREEN_WIDTH-BLOCK_THICK)||(points[i].x<BLOCK_THICK)||(points[i].y>LOGICAL_SCREEN_HEIGHT-BLOCK_THICK)||(points[i].y<BLOCK_THICK))
                return true;
        }
        if ((points[8].x>LOGICAL_SCREEN_WIDTH-BLOCK_THICK)||(points[8].x<BLOCK_THICK)||(points[8].y>LOGICAL_SCREEN_HEIGHT-BLOCK_THICK)||(points[8].y<BLOCK_THICK))
            return true;
        return false;
    }
    bool collideWithPoint(float x,float y){
        Point2f x1(x-points[0].x,y-points[0].y),x2(x-points[1].x,y-points[1].y),x3(x-points[8].x,y-points[8].y),x4(x-points[2].x,y-points[2].y),x5(x-points[3].x,y-points[3].y);
        float r1 = x1*x2, r2 = x2*x3, r3 = x3*x4, r4 = x4*x5, r5 = x5*x1;
        if ((r1>0 && r2>0 && r3>0 && r4>0 && r5>0) || (r1<0 && r2<0 && r3<0 && r4<0 && r5<0)) return true;
        else return false;
    }
    bool collideWithBullet(Point2f &bullet_pos){
        if (abs(bullet_pos.x-center.x)>TANK_CANNON_RADIUS || abs(bullet_pos.y-center.y)>TANK_CANNON_RADIUS)
            return false;
        return sqrt((bullet_pos.x-center.x)*(bullet_pos.x-center.x)+(bullet_pos.y-center.y)*(bullet_pos.y-center.y))<TANK_CANNON_RADIUS;
    }
    bool checkBulletCollision(){
        if (buff.protective_buff == BUFF_SHELL && buff.shell_open) return false;
        for (Bullet &p: bullets->que){
            if (collideWithBullet(p.pos)){
                return true;
            }
        }
        #ifdef USE_CUDA
        if (bullets->gatling_que.checkCollision(center))
            return true;
        #else
        for (GatlingBullet &p: bullets->gatling_que){
            if (collideWithBullet(p.pos)){
                p.life = 0;
                return true;
            }
        }
        #endif
        for (PierceBullet &p: bullets->pierce_que){
            if (collideWithBullet(p.pos1) || collideWithBullet(p.pos2)){
                p.alive = false;
                return true;
            }
        }
        return false;
    }
    bool collideWithBlock(SDL_Rect &rect,int type){
        for (int i=0;i<4;i++){
            if ((points[i].x>rect.x)&&(points[i].x<rect.x+rect.w)&&(points[i].y>rect.y)&&(points[i].y<rect.y+rect.h))
                return true;
        }
        if ((points[8].x>rect.x)&&(points[8].x<rect.x+rect.w)&&(points[8].y>rect.y)&&(points[8].y<rect.y+rect.h))
            return true;
        if (type == BLOCK_HORIZ){
            if (collideWithPoint(rect.x,rect.y+rect.h/2) || collideWithPoint(rect.x+rect.w,rect.y+rect.h/2))
                return true;
        }
        if (type == BLOCK_VERTI){
            if (collideWithPoint(rect.x+rect.w/2,rect.y) || collideWithPoint(rect.x+rect.w/2,rect.y+rect.h))
                return true;
        }
        return false;
    }
    bool collideWithLine(Point2f p1,Point2f p2,int type){
        return lineWithLine(points[0],points[1],p1,p2) ||
               lineWithLine(points[1],points[8],p1,p2) ||
               lineWithLine(points[8],points[2],p1,p2) ||
               lineWithLine(points[2],points[3],p1,p2) ||
               lineWithLine(points[3],points[0],p1,p2);
    }
    bool collide(){
        int x_block = int(center.x/MAP_BLOCK_SIZE);
        int y_block = int(center.y/MAP_BLOCK_SIZE);
        if (x_block<1 || x_block==MAP_BLOCK_COLS-1 || y_block<1 || y_block==MAP_BLOCK_ROWS-1)
            if (outOfMap()) return true;
        bool is_collide = false;
        for (int y=y_block-1;y<=y_block+1;y++){
            for (int x=x_block-1;x<=x_block+1;x++){
                if (map->data[y][x] == BLOCK_HORIZ){
                    is_collide = is_collide || collideWithBlock(map->collide_rects[y][x],BLOCK_HORIZ);
                }else if (map->data[y][x] == BLOCK_VERTI){
                    is_collide = is_collide || collideWithBlock(map->collide_rects[y][x],BLOCK_VERTI);
                }
            }
        }
        return is_collide;
    }
    bool teleportCollide(){
        int x_block = int(center.x/MAP_BLOCK_SIZE);
        int y_block = int(center.y/MAP_BLOCK_SIZE);
        bool is_collide = false;
        for (int y=y_block-1;y<=y_block+1;y++){
            for (int x=x_block-1;x<=x_block+1;x++){
                if (map->data[y][x] == BLOCK_HORIZ){
                    is_collide = is_collide || collideWithLine(Point2f(MAP_BLOCK_SIZE*x,MAP_BLOCK_SIZE*(y+1)),Point2f(MAP_BLOCK_SIZE*x+BLOCK_LENGTH,MAP_BLOCK_SIZE*(y+1)),BLOCK_HORIZ);
                }else if (map->data[y][x] == BLOCK_VERTI){
                    is_collide = is_collide || collideWithLine(Point2f(MAP_BLOCK_SIZE*(x+1),MAP_BLOCK_SIZE*y),Point2f(MAP_BLOCK_SIZE*(x+1),MAP_BLOCK_SIZE*y+BLOCK_LENGTH),BLOCK_VERTI);
                }
            }
        }
        return is_collide;
    }
    void rotateClockwise(){
        if (buff.protective_buff == BUFF_TELEPORT && buff.teleport_stuck)
            return;
        angle += TANK_DEFAULT_ROTATE_SPEED;
        getPoints();
        if (collide()){
            angle-=TANK_DEFAULT_ROTATE_SPEED;
        }
        if (angle>PI*2) angle-=PI*2;
    }
    void rotateClockwiseSlow(){
        if (buff.protective_buff == BUFF_TELEPORT && buff.teleport_stuck)
            return;
        angle += TANK_PRECISE_ROTATE_SPEED;
        getPoints();
        if (collide()){
            angle-=TANK_PRECISE_ROTATE_SPEED;
        }
        if (angle>PI*2) angle-=PI*2;
    }
    void rotateAnticlockwise(){
        if (buff.protective_buff == BUFF_TELEPORT && buff.teleport_stuck)
            return;
        angle -= TANK_DEFAULT_ROTATE_SPEED;
        getPoints();
        if (collide()){
            angle+=TANK_DEFAULT_ROTATE_SPEED;
        }
        if (angle<0) angle+=PI*2;
    }
    void rotateAnticlockwiseSlow(){
        if (buff.protective_buff == BUFF_TELEPORT && buff.teleport_stuck)
            return;
        angle -= TANK_PRECISE_ROTATE_SPEED;
        getPoints();
        if (collide()){
            angle+=TANK_PRECISE_ROTATE_SPEED;
        }
        if (angle<0) angle+=PI*2;
    }
    void moveForward(){
        if (buff.protective_buff == BUFF_TELEPORT && buff.teleport_stuck)
            return;
        float y = -TANK_DEFAULT_MOVE_SPEED*std::sin(angle),
              x = TANK_DEFAULT_MOVE_SPEED*std::cos(angle);
        center.x+=x;
        getPoints();
        if (collide()) center.x-=x;
        center.y+=y;
        getPoints();
        if (collide()) center.y-=y;
    }
    void moveBackward(){
        if (buff.protective_buff == BUFF_TELEPORT && buff.teleport_stuck)
            return;
        float y = -TANK_DEFAULT_MOVE_SPEED*std::sin(angle),
              x = TANK_DEFAULT_MOVE_SPEED*std::cos(angle);
        center.x-=x;
        getPoints();
        if (collide()) center.x+=x;
        center.y-=y;
        getPoints();
        if (collide()) center.y+=y;
    }
    void drawCooldown(Interface &screen){
        if (buff.protective_buff != BUFF_NONE){
            Sint16 end = (now-buff.protective_cooldown<BUFF_COOLDOWNS[buff.protective_buff])?360*(now-buff.protective_cooldown)/BUFF_COOLDOWNS[buff.protective_buff]:359;
            filledPieColor(screen.renderer, center.x,center.y, TANK_CANNON_RADIUS-TANK_LINE_THICK, 0, end, COOLDOWN_COLOR[color]);
        }
    }
    void draw(Interface &screen){
        //body
        filledTrigonColor(screen.renderer,points[0].x,points[0].y,points[1].x,points[1].y,points[2].x,points[2].y,TANK_COLOR[color]);
        filledTrigonColor(screen.renderer,points[2].x,points[2].y,points[3].x,points[3].y,points[0].x,points[0].y,TANK_COLOR[color]);

        for (int i=0;i<4;i++){
            thickLineColor(screen.renderer,points[i].x,points[i].y,points[(i+1)%4].x,points[(i+1)%4].y,TANK_LINE_THICK,COLOR_WHITE);
        }
        //cannon
        filledTrigonColor(screen.renderer,points[4].x,points[4].y,points[5].x,points[5].y,points[6].x,points[6].y,TANK_COLOR[color]);
        filledTrigonColor(screen.renderer,points[6].x,points[6].y,points[7].x,points[7].y,points[4].x,points[4].y,TANK_COLOR[color]);
            
        thickLineColor(screen.renderer,points[4].x,points[4].y,points[5].x,points[5].y,TANK_LINE_THICK,COLOR_WHITE);
        thickLineColor(screen.renderer,points[6].x,points[6].y,points[7].x,points[7].y,TANK_LINE_THICK,COLOR_WHITE);

        filledCircleColor(screen.renderer,center.x,center.y, TANK_CANNON_RADIUS, COLOR_WHITE);
        filledCircleColor(screen.renderer,center.x,center.y, TANK_CANNON_RADIUS-TANK_LINE_THICK-1, TANK_COLOR[color]);

        drawCooldown(screen);
        if (buff.shell_open){
            aacircleColor(screen.renderer,center.x,center.y,1.5*TANK_BODY_HEIGHT,addAlpha(SHELL_COLOR[color],80));
            filledCircleColor(screen.renderer,center.x,center.y,1.5*TANK_BODY_HEIGHT,addAlpha(SHELL_COLOR[color],80));
        }
    }
    void teleportForward(){
        if (buff.protective_buff != BUFF_TELEPORT) return;
        if (now-buff.protective_cooldown < BUFF_COOLDOWNS[BUFF_TELEPORT]) return;
        float y = -BUFF_TELEPORT_DISTANCE*std::sin(angle),
              x = BUFF_TELEPORT_DISTANCE*std::cos(angle);
        center.x+=x;
        center.y+=y;
        buff.protective_cooldown = now;
        getPoints();
        if (teleportCollide()||collide()){
            buff.teleport_stuck = true;
            special_effect->chain_que.push_back(Chain(now,points[0],points[2],color,buff.teleport_stuck));
        }else{
            buff.teleport_stuck = false;
        }
        if (outOfMap()){
            center.x-=x;
            center.y-=y;
            getPoints();
        }else{
            special_effect->tele_que.push_back(TeleportShade(angle,(center-Point2f(x,y)),color));
        }
    }
    void teleportBackward(){
        if (buff.protective_buff != BUFF_TELEPORT) return;
        if (now-buff.protective_cooldown < BUFF_COOLDOWNS[BUFF_TELEPORT]) return;
        float y = -BUFF_TELEPORT_DISTANCE*std::sin(angle),
              x = BUFF_TELEPORT_DISTANCE*std::cos(angle);
        center.x-=x;
        center.y-=y;
        buff.protective_cooldown = now;
        getPoints();
        if (teleportCollide()||collide()){
            buff.teleport_stuck = true;
            special_effect->chain_que.push_back(Chain(now,points[0],points[2],color,buff.teleport_stuck));
        }else{
            buff.teleport_stuck = false;
        }
            
        if (outOfMap()){
            center.x+=x;
            center.y+=y;
            getPoints();
        }else{
            special_effect->tele_que.push_back(TeleportShade(angle,(center+Point2f(x,y)),color));
        }
    }
    void openShell(){
        if (buff.protective_buff != BUFF_SHELL || buff.shell_open) return;
        if (now-buff.protective_cooldown < BUFF_COOLDOWNS[BUFF_SHELL]) return;
        buff.shell_open = true;
        buff.protective_last = now;
    }
    void update(Uint32 mouse_button,float mouse_x,float mouse_y,Uint8* keysArray,bool to_fire = true){
        if (control == LEFT_KEYBOARD){
            if (keysArray[SDL_SCANCODE_W] && !keysArray[SDL_SCANCODE_S]) moveForward();
            else if (keysArray[SDL_SCANCODE_S] && !keysArray[SDL_SCANCODE_W]) moveBackward();

            if (keysArray[SDL_SCANCODE_LSHIFT]){
                if (keysArray[SDL_SCANCODE_A] && !keysArray[SDL_SCANCODE_D]) rotateClockwiseSlow();
                else if (keysArray[SDL_SCANCODE_D] && !keysArray[SDL_SCANCODE_A]) rotateAnticlockwiseSlow();
            }else{
                if (keysArray[SDL_SCANCODE_A] && !keysArray[SDL_SCANCODE_D]) rotateClockwise();
                else if (keysArray[SDL_SCANCODE_D] && !keysArray[SDL_SCANCODE_A]) rotateAnticlockwise();
            }
            if (keysArray[SDL_SCANCODE_I]){
                openShell();
                if (!keysArray[SDL_SCANCODE_S]) teleportForward();
                else teleportBackward();
            }
            if (to_fire && keysArray[SDL_SCANCODE_J]){
                fireGatling();
                firePierce();
                fire();
            }
        }else if (control == RIGHT_KEYBOARD){
            if (keysArray[SDL_SCANCODE_UP] && !keysArray[SDL_SCANCODE_DOWN]) moveForward();
            else if (keysArray[SDL_SCANCODE_DOWN] && !keysArray[SDL_SCANCODE_UP]) moveBackward();
            
            if (keysArray[SDL_SCANCODE_RCTRL]){
                if (keysArray[SDL_SCANCODE_LEFT] && !keysArray[SDL_SCANCODE_RIGHT]) rotateClockwiseSlow();
                else if (keysArray[SDL_SCANCODE_RIGHT] && !keysArray[SDL_SCANCODE_LEFT]) rotateAnticlockwiseSlow();
            }else{
                if (keysArray[SDL_SCANCODE_LEFT] && !keysArray[SDL_SCANCODE_RIGHT]) rotateClockwise();
                else if (keysArray[SDL_SCANCODE_RIGHT] && !keysArray[SDL_SCANCODE_LEFT]) rotateAnticlockwise();
            }
            if (keysArray[SDL_SCANCODE_KP_0]){
                openShell();
                if (!keysArray[SDL_SCANCODE_DOWN]) teleportForward();
                else teleportBackward();
            }
            if (to_fire && keysArray[SDL_SCANCODE_KP_ENTER]){
                fireGatling();
                firePierce();
                fire();
            }
        }else if (control == MOUSE){
            float dx = mouse_x-center.x, dy = mouse_y-center.y;
            float a = -atan(dy/dx);
            if (dx<0) a+=PI;
            if (a<0) a+=PI*2;
            if (a>PI*2) a-=PI*2;

            float radius = sqrt(dx*dx+dy*dy);
            float dr = abs(a - angle);
            if (dr>2*PI) dr-=2*PI;

            bool backward = false;
            if (radius>CONTROL_RADIUS_PRECISE){
                if (2*dr<PI || 2*dr>3*PI){
                    if ((a>angle && 2*dr<PI)||(a<angle && 2*dr>3*PI)) rotateClockwise();
                    else rotateAnticlockwise();
                    if (!(mouse_button & 2)){
                        moveForward();
                    }
                }else{
                    backward = true;
                    if (!(mouse_button & 2)){
                        moveBackward();
                    }
                }
            }
            if (to_fire && mouse_button & 1){
                fireGatling();
                firePierce();
                fire();
            }
            if (mouse_button & 4){
                openShell();
                if (!backward) teleportForward();
                else teleportBackward();
            }
        }
        if (buff.protective_buff == BUFF_TELEPORT && !teleportCollide() && !collide()){
            buff.teleport_stuck = false;
        }
        //check elascape time and close shell
        else if (buff.protective_buff == BUFF_SHELL && buff.shell_open){
            if (now-buff.protective_last>=BUFF_SHELL_LAST){
                buff.shell_open = false;
                buff.protective_cooldown = now;
            }
        }
    }
};