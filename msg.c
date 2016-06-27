//
// Created by TianYi Zhuang on 6/25/16.
//

#include "msg.h"
#include "types.h"
#include "defs.h"

int handleMessage(message* msg)
{
    guiKernelHandleMsg(msg);
    return 0;
}
