#include "config.hpp"

void randomTwoPoints(Point2f &A,Point2f &B){
    float xA = float((0.5+rand()%(MAP_BLOCK_COLS-2))*MAP_BLOCK_SIZE), yA = float((0.5+rand()%(MAP_BLOCK_ROWS-2))*MAP_BLOCK_SIZE);
    float xB = float((0.5+rand()%(MAP_BLOCK_COLS-2))*MAP_BLOCK_SIZE), yB = float((0.5+rand()%(MAP_BLOCK_ROWS-2))*MAP_BLOCK_SIZE);
    if (xB==xA && yB==yA){
        xB = float((0.5+rand()%(MAP_BLOCK_COLS-2))*MAP_BLOCK_SIZE), yB = float((0.5+rand()%(MAP_BLOCK_ROWS-2))*MAP_BLOCK_SIZE);
    }
    A.x = xA,A.y = yA;
    B.x = xB,B.y = yB;
};

bool lineWithLine(Point2f p1,Point2f p2,Point2f &p3,Point2f &p4){
    Point2f x1(p1.x-p3.x,p1.y-p3.y),x2(p1.x-p4.x,p1.y-p4.y),x3(p2.x-p3.x,p2.y-p3.y),x4(p2.x-p4.x,p2.y-p4.y);
    float r1 = x1*x2, r2 = x3*x4, r3 = x1*x3, r4 = x2*x4;
    return (((r1>0)^(r2>0)) && ((r3>0)^(r4>0)));
};

Uint32 toGfxColor(unsigned char r,unsigned char g,unsigned char b,unsigned char a){
    Uint32 res = a;
    res<<=8;
    res+=b;
    res<<=8;
    res+=g;
    res<<=8;
    res+=r;
    return res;
};

Uint32 addAlpha(const Uint32 &color, unsigned char a){
    Uint32 res=a;
    res<<=24;
    res+=color&(0xFFFFFF);
    return res;
};