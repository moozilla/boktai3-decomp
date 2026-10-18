#include "global.h"

struct S {
    u8 pad[0x38];
    u32 f38;
    u32 f3c;
    u8 pad2[0x3a0 - 0x40];
    u32 f3a0;
    u32 f3a4;
};
struct S *FrostEmitter_CreateTask(void);

struct S *FrostEmitter_Create(void)
{
    struct S *p = FrostEmitter_CreateTask();
    u32 z = 0;
    p->f38 = z;
    p->f3c = z;
    p->f3a0 = z;
    p->f3a4 = z;
    return p;
}
