#pragma once
#ifdef USE_CUDA
#include "gatling_gpu.hpp"
#endif
#include "bullet.hpp"

struct Bullets{
    std::deque<Bullet> que;
    std::deque<GatlingBullet> gatling_que;
    std::deque<PierceBullet> pierce_que;

    Bullets():gatling_que(){}
    void update(unsigned long long &now,Map &map){
        while (!que.empty() && !que.front().checkAlive(now)){
            que.pop_front();
        }
        for (Bullet &p: que){
            p.update(map);
        }
        while (!gatling_que.empty() && gatling_que.front().life<=0){
            gatling_que.pop_front();
        }
        for (GatlingBullet &b: gatling_que){
            b.update(map);
        }
        while (!pierce_que.empty() && !pierce_que.front().alive){
            pierce_que.pop_front();
        }
        for (PierceBullet &p: pierce_que){
            p.update();
        }
    }
    void draw(Interface &screen){
        for (Bullet &b:que){
            filledCircleRGBA (screen.renderer,b.pos.x,b.pos.y, BULLET_RADIUS, 255,255,255,255);
        }
        int count = 0;
        SDL_Vertex vertices[gatling_que.size()*6];
        for (GatlingBullet &b:gatling_que) if (b.life>0){
            vertices[count++] = {{b.pos.x-BULLET_RADIUS, b.pos.y-BULLET_RADIUS}, {b.r,b.g,b.b,255}, {1, 1}};
            vertices[count++] = {{b.pos.x+BULLET_RADIUS, b.pos.y-BULLET_RADIUS}, {b.r,b.g,b.b,255}, {1, 1}};
            vertices[count++] = {{b.pos.x-BULLET_RADIUS, b.pos.y+BULLET_RADIUS}, {b.r,b.g,b.b,255}, {1, 1}};

            vertices[count++] = {{b.pos.x+BULLET_RADIUS, b.pos.y+BULLET_RADIUS}, {b.r,b.g,b.b,255}, {1, 1}};
            vertices[count++] = {{b.pos.x+BULLET_RADIUS, b.pos.y-BULLET_RADIUS}, {b.r,b.g,b.b,255}, {1, 1}};
            vertices[count++] = {{b.pos.x-BULLET_RADIUS, b.pos.y+BULLET_RADIUS}, {b.r,b.g,b.b,255}, {1, 1}};
        }
        SDL_RenderGeometry(screen.renderer, NULL, vertices, count, NULL, 0);
        for (PierceBullet &b:pierce_que){
            if (b.alive)
                thickLineRGBA(screen.renderer,b.pos1.x,b.pos1.y,b.pos2.x,b.pos2.y,BULLET_RADIUS*2,255,255,255,255);
        }
    }
};