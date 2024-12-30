#include "../include/map.hpp"
#include "../include/menu.hpp"
#include "../include/tank.hpp"
#include "../include/score.hpp"
#include "../include/begin.hpp"
#include "../include/config.hpp"
#include "../include/interface.hpp"

#include <chrono>
void victoryAnimation(Interface &screen,Tank &tank,Map &map,Score &score,Bullets &bullets,SpecialEffect special_effect,Explosion exp,unsigned long long &now){
    score.generate(screen);
    Uint8* keysArray = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
    int mouse_x,mouse_y;
    Uint32 mouse_button = SDL_GetMouseState(&mouse_x,&mouse_y);

    unsigned long long diff;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(), end = std::chrono::steady_clock::now();
    while (true){
        now++;
        start = std::chrono::steady_clock::now();
        SDL_PumpEvents();
        if (keysArray[SDL_SCANCODE_ESCAPE]){
            exit(0);
        }
        screen.drawBackground();
        score.draw(screen);
        map.draw(screen);
        tank.update(mouse_button,mouse_x,mouse_y,keysArray);
        tank.draw(screen);
        if (exp.updateAndDraw(now,screen)) break;
        special_effect.updateAndDraw(now,screen);
        bullets.update(now,map);
        bullets.draw(screen);
        screen.present();
        end = std::chrono::steady_clock::now();
        diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        if (diff < FRAME_LATENCY)
            usleep(FRAME_LATENCY-diff);
    }
    map.generate();
    now = 0;
}
//860 - 1040 µs
//pressure: 2300 µs
int main(){
    Interface screen;
    Begin begin(screen.renderer,"../resource/begin/logo-Rockstar-Games.png",255, 170, 51);
    begin.display();
    Score score(screen, "../resource/font/E4DigitalLowercases-wZEn.ttf");
    Map map;
    Menu menu;
    SpecialEffect special_effect;

    next:
    unsigned long long now = 0;
    Bullets bullets;
    Point2f pA,pB;
    randomTwoPoints(pA,pB);

    Tank tankA((float)(2*PI*rand())/(RAND_MAX),pA, RED, RIGHT_KEYBOARD, &map, &bullets, &special_effect);
    tankA.buff.protective_buff = BUFF_TELEPORT;
    tankA.buff.aggressive_buff = BUFF_PIERCE;
    Tank tankB((float)(2*PI*rand())/(RAND_MAX),pB, BLUE, LEFT_KEYBOARD, &map, &bullets, &special_effect);
    tankB.buff.protective_buff = BUFF_TELEPORT;
    tankB.buff.aggressive_buff = BUFF_GATLING;
    score.step=0;
    score.getCountdown(screen,now);
    int mouse_x,mouse_y;
    Uint32 mouse_button = SDL_GetMouseState(&mouse_x,&mouse_y);
    Uint8* keysArray = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));

    unsigned long long diff;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(), end = std::chrono::steady_clock::now();
    while (true){
        now++;
        start = std::chrono::steady_clock::now();
        SDL_PumpEvents();
        if (keysArray[SDL_SCANCODE_ESCAPE]){
            exit(0);
        }
        tankA.getTime(now);
        tankB.getTime(now);

        screen.drawBackground();
        if (score.getCountdown(screen,now)){
            score.generate(screen);
            break;
        }
        score.draw(screen);

        map.draw(screen);

        tankA.update(mouse_button,mouse_x,mouse_y,keysArray,false);
        tankA.draw(screen);

        tankB.update(mouse_button,mouse_x,mouse_y,keysArray,false);
        tankB.draw(screen);

        special_effect.updateAndDraw(now,screen);
        screen.present();
        end = std::chrono::steady_clock::now();
        diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        if (diff < FRAME_LATENCY)
            usleep(FRAME_LATENCY-diff);
    }
    while (true){
        now++;
        start = std::chrono::steady_clock::now();
        tankA.getTime(now);
        tankB.getTime(now);
        mouse_button = SDL_GetMouseState(&mouse_x,&mouse_y);
        SDL_PumpEvents();
        Uint8* keysArray = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));

        if (keysArray[SDL_SCANCODE_ESCAPE]){
            exit(0);
        }
        screen.drawBackground();
        score.draw(screen);

        map.draw(screen);

        tankA.update(mouse_button,mouse_x,mouse_y,keysArray);
        tankA.draw(screen);

        tankB.update(mouse_button,mouse_x,mouse_y,keysArray);
        tankB.draw(screen);

        special_effect.updateAndDraw(now,screen);

        bullets.update(now,map);
        bullets.draw(screen);

        if (tankA.checkBulletCollision()){
            score.right++;
            victoryAnimation(screen,tankB,map,score,bullets,special_effect,Explosion(tankA.center,now),now);
            goto next;
        }
        if (tankB.checkBulletCollision()){
            score.left++;
            victoryAnimation(screen,tankA,map,score,bullets,special_effect,Explosion(tankB.center,now),now);
            goto next;
        }
        screen.present();
        end = std::chrono::steady_clock::now();
        diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        if (diff < FRAME_LATENCY)
            usleep(FRAME_LATENCY-diff);
        std::cout<< "Time difference = " << diff << "[µs]" << std::endl;
    }
    screen.close();
    return 0;
    goto next;
}