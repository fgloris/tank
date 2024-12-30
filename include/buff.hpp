#ifndef __BUFF_HEADER_
#define __BUFF_HEADER_
#include "config.hpp"

const float BUFF_TELEPORT_DISTANCE = MAP_BLOCK_SIZE;
const float BUFF_SHELL_LAST = 1000;
const float BUFF_PIERCE_AIM_LENGTH = 500;
#define BUFF_TELEPORT_RGBA 255,200,0,150

const uint64_t BUFF_COOLDOWNS[5] = {400,1500,1500,20,1000};

enum TankBuff{
    BUFF_NONE=0,
    BUFF_TELEPORT=1,BUFF_SHELL=2,
    BUFF_GATLING=3,BUFF_PIERCE=4
};

struct BuffParams{
    uint64_t protective_cooldown;
    uint64_t protective_last;
    uint64_t aggressive_cooldown;
    bool teleport_stuck = false;
    bool shell_open = false;
    TankBuff protective_buff = BUFF_NONE;
    TankBuff aggressive_buff = BUFF_NONE;
    BuffParams(){
        aggressive_cooldown = protective_cooldown = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

#endif