#ifndef __SEPCIAL_HEADER_
#define __SEPCIAL_HEADER_
#include "config.hpp"

struct Smoke{
    Point2f pos;
    float radius,alpha;
    Point2f dpos;
    bool alive = true;
    Smoke(Point2f _pos,Point2f &ori_pos,float _radius,float _alpha):pos(_pos),radius(_radius),alpha(_alpha){
        dpos = _pos-ori_pos;
        dpos.toUnit();
        dpos*=0.01;
    }
    void updateAndDraw(Interface &screen){
        if (!alive) return;
        radius+=0.01;
        alpha-=0.1;
        pos+=dpos;
        filledCircleRGBA(screen.renderer,pos.x,pos.y,radius,255,255,255,alpha);
        if (radius>50 || alpha<5) alive = false;
    }
};

struct Piece{
    Point2f pos;
    Point2f dpos;
    int smoke_count = 0;
    int cooldown = 0, life;
    bool alive = true;
    std::deque<Smoke> *smoke_que;
    Piece(int now, int _life, Point2f _pos,Point2f &ori_pos,std::deque<Smoke>* _smoke_que):pos(_pos),life(_life),cooldown(now){
        dpos = _pos-ori_pos;
        dpos.toUnit();
        dpos*=1;
        smoke_que = _smoke_que;
    }
    void updateAndDraw(int now){
        if (!alive) return;
        pos+=dpos;
        if (now - cooldown>20){
            cooldown = now;
            smoke_count++;
            smoke_que->push_back(Smoke(Point2f(pos.x+random(20),pos.y+random(20)),pos,20+random(8),100+random(10)));
        }
        if (smoke_count>life) alive = false;
    }
};

struct Explosion{
    Uint32 piece_color = toGfxColor(255,0,0,255);
    Point2f smoke_ori_pos;
    std::deque<Smoke> smoke_que;
    std::deque<Piece> piece_que;
    Explosion(Point2f &_pos,int now){
        smoke_ori_pos = _pos;
        smoke_que.push_back(Smoke(Point2f(_pos.x+random(50),_pos.y+random(50)),_pos,40+random(8),120+random(10)));
        smoke_que.push_back(Smoke(Point2f(_pos.x+random(50),_pos.y+random(50)),_pos,40+random(8),120+random(10)));
        smoke_que.push_back(Smoke(Point2f(_pos.x+random(50),_pos.y+random(50)),_pos,40+random(8),120+random(10)));
        smoke_que.push_back(Smoke(Point2f(_pos.x+random(50),_pos.y+random(50)),_pos,40+random(8),120+random(10)));
        smoke_que.push_back(Smoke(Point2f(_pos.x+random(50),_pos.y+random(50)),_pos,40+random(8),120+random(10)));
        smoke_que.push_back(Smoke(Point2f(_pos.x+random(50),_pos.y+random(50)),_pos,40+random(8),120+random(10)));

        piece_que.push_back(Piece(now,6+random(4),Point2f(_pos.x+(random(80)*2),_pos.y+(random(80)*2)),_pos,&smoke_que));
        piece_que.push_back(Piece(now,10+random(4),Point2f(_pos.x+random(50),_pos.y+random(50)),_pos,&smoke_que));
        piece_que.push_back(Piece(now,10+random(4),Point2f(_pos.x+random(50),_pos.y+random(50)),_pos,&smoke_que));
    }
    bool updateAndDraw(int now,Interface &screen){
        while (!smoke_que.front().alive && !smoke_que.empty()){
            smoke_que.pop_front();
        }
        for (Smoke &s:smoke_que){
            s.updateAndDraw(screen);
        }
        for (Piece &p:piece_que){
            p.updateAndDraw(now);
        }
        if (smoke_que.empty()) return true;
        return false;
    }
};

struct Chain{
    ColorSystem color;
    Point2f p1, p2;
    Point2f chain1,chain2,chain3,chain4,chain5,chain6,increase;
    float dis,distance;
    bool &teleport_stuck;
    int time;
    Chain(int now,const Point2f &_p1,const Point2f &_p2, ColorSystem _color, bool &_teleport_stuck):p1(_p1),p2(_p2),teleport_stuck(_teleport_stuck){
        time = now;
        distance = p2^p1;
        dis = 0;
        color = _color;
        float x_distance = p2.x-p1.x, y_distance = p2.y-p1.y;
        float rate = SPECIAL_CHAIN_WIDTH/distance;
        float x_rate = x_distance/distance, y_rate = y_distance/distance;

        Point2f p3(p1.x+rate*y_distance,p1.y-rate*x_distance), p4(p1.x-rate*y_distance,p1.y+rate*x_distance);
        chain1 = Point2f(p1.x,p1.y);
        chain2 = Point2f(p1.x+x_rate*(SPECIAL_CHAIN_NODE_LENGTH),p1.y+y_rate*(SPECIAL_CHAIN_NODE_LENGTH));

        chain3 = Point2f(p3.x+x_rate*(SPECIAL_CHAIN_NODE_STEP1),p3.y+y_rate*(SPECIAL_CHAIN_NODE_STEP1));
        chain4 = Point2f(p3.x+x_rate*(SPECIAL_CHAIN_NODE_STEP2),p3.y+y_rate*(SPECIAL_CHAIN_NODE_STEP2));

        chain5 = Point2f(p4.x+x_rate*(SPECIAL_CHAIN_NODE_STEP1),p4.y+y_rate*(SPECIAL_CHAIN_NODE_STEP1));
        chain6 = Point2f(p4.x+x_rate*(SPECIAL_CHAIN_NODE_STEP2),p4.y+y_rate*(SPECIAL_CHAIN_NODE_STEP2));
        increase = Point2f(x_rate*(SPECIAL_CHAIN_LENGTH),y_rate*(SPECIAL_CHAIN_LENGTH));

        chain1-=increase;
        chain2-=increase;
        chain3-=increase;
        chain4-=increase;
        chain5-=increase;
        chain6-=increase;
    }
    void updateAndDraw(int now,Interface &screen){
        if (!teleport_stuck) return;
        if ((dis < distance*1.2) && (now - time > 50)){
            dis+=SPECIAL_CHAIN_LENGTH;
            time = now;
        }
        Point2f
        chain1_ = chain1,
        chain2_ = chain2,
        chain3_ = chain3,
        chain4_ = chain4,
        chain5_ = chain5,
        chain6_ = chain6;

        for (float i=0;i<dis;i+=SPECIAL_CHAIN_LENGTH){
            thickLineColor(screen.renderer,chain1_.x,chain1_.y,chain2_.x,chain2_.y,SPECIAL_CHAIN_THICK,SPECIAL_CHAIN_COLOR[color]);
            thickLineColor(screen.renderer,chain3_.x,chain3_.y,chain4_.x,chain4_.y,SPECIAL_CHAIN_THICK,SPECIAL_CHAIN_COLOR[color]);
            thickLineColor(screen.renderer,chain5_.x,chain5_.y,chain6_.x,chain6_.y,SPECIAL_CHAIN_THICK,SPECIAL_CHAIN_COLOR[color]);
            chain1_+=increase;
            chain2_+=increase;
            chain3_+=increase;
            chain4_+=increase;
            chain5_+=increase;
            chain6_+=increase;
        }
        thickLineColor(screen.renderer,chain1_.x,chain1_.y,chain2_.x,chain2_.y,SPECIAL_CHAIN_THICK,SPECIAL_CHAIN_COLOR[color]);
    }   
};

struct TeleportShade{
    ColorSystem color;
    Point2f center = {0,0};
    Point2f vertices[8] = {
        Point2f(-(TANK_BODY_WIDTH/2),-(TANK_BODY_HEIGHT/2)),Point2f(-(TANK_BODY_WIDTH/2),(TANK_BODY_HEIGHT/2)),Point2f((TANK_BODY_WIDTH/2),(TANK_BODY_HEIGHT/2)),Point2f((TANK_BODY_WIDTH/2),-(TANK_BODY_HEIGHT/2)),
        Point2f(-(TANK_CANNON_THICK)/2,(TANK_CANNON_LENGTH)),Point2f(-(TANK_CANNON_THICK)/2,TANK_CANNON_RADIUS),Point2f((TANK_CANNON_THICK)/2,TANK_CANNON_RADIUS),Point2f((TANK_CANNON_THICK)/2,(TANK_CANNON_LENGTH))};
    SDL_Point points[8];
    Sint16 vx[8],vy[8];
    float angle,alpha;
    void getPoints(){
        for (int i=0;i<8;i++){
            points[i] = vertices[i].toCartesian(angle,center);
        }
        for (int i=0;i<8;i++){
            vx[i]=points[i].x;
            vy[i]=points[i].y;
        }
    }
    TeleportShade(float _angle, Point2f _center, ColorSystem _color){
        angle = _angle;
        center = _center;
        color = _color;
        for (Point2f &p:vertices)
            p.toPolar();
        getPoints();
        alpha = 100;
    }
    void updateAndDraw(Interface &screen){
        if (alpha<1) return;
        alpha-=0.2;
        filledPolygonColor(screen.renderer, vx, vy, 4, addAlpha(TANK_COLOR[color],alpha));
        for (int i=0;i<4;i++){
            thickLineColor(screen.renderer,points[i].x,points[i].y,points[(i+1)%4].x,points[(i+1)%4].y,TANK_LINE_THICK,addAlpha(COLOR_WHITE,alpha));
        }
        //cannon
        filledPolygonColor(screen.renderer, vx+4,vy+4, 4, addAlpha(TANK_COLOR[color],alpha));
        thickLineColor(screen.renderer,points[4].x,points[4].y,points[5].x,points[5].y,TANK_LINE_THICK,addAlpha(COLOR_WHITE,alpha));
        thickLineColor(screen.renderer,points[6].x,points[6].y,points[7].x,points[7].y,TANK_LINE_THICK,addAlpha(COLOR_WHITE,alpha));
        for (int i=0;i<TANK_LINE_THICK;i++)
            circleColor(screen.renderer,center.x,center.y, TANK_CANNON_RADIUS-i, addAlpha(COLOR_WHITE,alpha));
    }
};

struct SpecialEffect{
    std::deque<TeleportShade> tele_que;
    std::deque<Chain> chain_que;
    std::deque<Smoke> smoke_que;
    SpecialEffect(){}
    bool updateAndDraw(int now,Interface &screen){
        while (!smoke_que.empty() && !smoke_que.front().alive){
            smoke_que.pop_front();
        }
        for (Smoke &s:smoke_que){
            s.updateAndDraw(screen);
        }
        while (!tele_que.empty() && !tele_que.front().alpha>1){
            tele_que.pop_front();
        }
        for (TeleportShade &s:tele_que){
            s.updateAndDraw(screen);
        }
        while (!chain_que.empty() && !chain_que.front().teleport_stuck){
            chain_que.pop_front();
        }
        for (Chain &c:chain_que){
            c.updateAndDraw(now,screen);
        }
        if (smoke_que.empty()) return true;

        return false;
    }
};

#endif