#include "global.h"
#include "render.h"
#include "ram.h"

static inline void andm(u32 *a, u32 m)
{
    *a &= m;
}
struct P {
    u8 f00[0x44];
    u32 w44;
    u8 f48[0x9c - 0x48];
    u8 s9c[0x56];
    u16 hf2;
};

s32 Collider_Refresh(void *);
void Task_Kill(void *);

void PlayerBurst_UpdateShutdown(struct P *p)
{
    u16 *c;
    p->w44 |= 1;
    c = &p->hf2;
    if (*c == 0) {
        andm(&gEnginePauseFlags, ~4);
        gEngineFlags &= ~1;
    }
    (*c)++;
    if (*c <= 5)
        Collider_Refresh(p->s9c);
    else
        Task_Kill(p);
}
