#include "global.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, enter;
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    u8 (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x6a];
    u8 stage;
    u32 counter;
    u8 pad184[0x1bf];
    u8 variant;
    s16 scale, minScale, maxScale, scaleStep;
    u8 pad34C[0x60];
    u32 mode;
} ActorAnimation;

static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    int shifted;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void sub_080297FC(void *, int, int, int);
void sub_08029E1C(void *);
int sub_08029E2C(void *);
void sub_08029AA8(void *);
void sub_0822B3BC(int);
void SceneActors_RunFirstEventScript(void *, void *);
void sub_08046834(void *);
void sub_081D5204(int);
void SceneActors_RunSecondEventScript(void *, void *);
static inline void effect(ActorAnimation *p, int mode, int id, int duration)
{
    *(u32 *)((u8 *)p + 0x1c0) = id;
    *((u8 *)p + 0x1bc) = mode;
    *((u8 *)p + 0x1bd) = duration;
    if (*((u8 *)p + 0x1bc) == 2) {
        u32 value = duration == 5 ? 1 : 2;
        *((u8 *)p + 0x1ba) = value;
    } else if (*((u8 *)p + 0x1bc) == 1)
        *((u8 *)p + 0x1ba) = 0;
}
void sub_0802A63C(ActorAnimation *p)
{
    ActorAnimation *base = p;
    if (p->changed) {
        p->changed = 0;
        sub_080297FC(p, 10, 1, 8);
        SceneActors_RunFirstEventScript(p, (u8 *)p + 0x8a4);
        sub_08046834(*(void **)((u8 *)p + 0x904));
        effect(p, 2, 349, 1);
        base->stage = 0;
        base->counter = 0;
        sub_0822B3BC(958);
    }
    if (p->padB2 && !base->stage) {
        sub_081D5204(80);
        SceneActors_RunSecondEventScript(p, (u8 *)base + 0x8a4);
        base->stage++;
    }
    {
        u32 mask = 13;
        p->flags |= mask;
    }
}
