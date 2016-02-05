#pragma once
#include <SEGGER_RTT_Conf.h>
#include <SEGGER_RTT.h>

/**
 * helper function to make rtt initialization easier
 */


#define RTT_INIT_UP_CH(ID, NAME, SZ, OPTION) do{\
    static char buf[SZ];\
    SEGGER_RTT_ConfigUpBuffer(ID,NAME,buf, SZ ,OPTION);/*TODO assert on fail*/\
}while(0)

#define RTT_INIT_DOWN_CH(ID, NAME, SZ, OPTION) do{\
    static char buf[SZ];\
    SEGGER_RTT_ConfigDownBuffer(ID,NAME,buf, SZ ,OPTION);/*TODO assert on fail*/\
}while(0)
