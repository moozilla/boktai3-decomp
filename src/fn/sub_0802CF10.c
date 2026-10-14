#include "global.h"
struct Object {
    u8 pad0[0xc];
    s16 pos[4];
    u8 pad14[0x3c];
    u32 flags, transient;
    u8 pad58[0xc4];
    void (*callback)(void *, int);
    u8 pad120[0x14];
    int ticks;
    u8 pad138[0x49];
    u8 facing;
    u8 pad182[2];
    u32 drawable;
};
void sub_0802CB94(struct Object *);
void DisplayAnimation_SetPosition(void *, void *);
void DisplayAnimation_UpdatePlayback(void *);
void sub_0802CF10(struct Object *p)
{
    void *d;
    sub_0802CB94(p);
    d = &p->drawable;
    DisplayAnimation_SetPosition(d, p->pos);
    {
        u32 mask = 2;
        if (p->flags & mask)
            DisplayAnimation_UpdatePlayback(d);
    }
}
