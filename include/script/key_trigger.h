#ifndef GUARD_SCRIPT_KEY_TRIGGER_H
#define GUARD_SCRIPT_KEY_TRIGGER_H

#include "global.h"

struct Args08197F68 {
    u16 count;
    s32 *values;
    s32 buf[8];
};
struct Task08197F68 {
    u8 pad0[0x18];
    u32 channel;
    void *script1;
    void *script2;
    u16 keys;
    u16 flags;
    struct Args08197F68 args;
};

#endif
