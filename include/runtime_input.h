#ifndef GUARD_RUNTIME_INPUT_H
#define GUARD_RUNTIME_INPUT_H
#include "runtime.h"
#include "gba/io_reg.h"
struct RuntimePadState {
    u16 held, pressed, released, reserved;
};
extern struct RuntimePadState gPadState[2];
s32 TaskSystem_UpdateRoot(void);
void TaskSystem_Initialize(void);
u16 String_Hash(u8 *text);
u32 Random_Next15(void);
#endif
