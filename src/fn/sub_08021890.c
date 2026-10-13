#include "global.h"
typedef struct {
    u8 pad[0xa3];
    u8 completed;
    u8 padA4[4];
    u8 facing;
    u8 padA9[13];
    u8 offset, mirror;
    u16 sequence;
} Actor;
void sub_08220D78(void *, int, u16, u16, u16);
void SceneActors_StartUnmirroredAnimation(Actor *p, u8 *animation, int resource, int sequence,
                                          int speed, int flags)
{
    p->offset = 0;
    p->mirror = 0;
    p->sequence = sequence;
    sub_08220D78(animation, resource, p->sequence, speed, flags);
    flags &= 1;
    if (!flags) {
        p->completed = 0;
    }
    if (p->mirror) {
        *(u32 *)(animation + 8) |= 4;
    } else {
        *(u32 *)(animation + 8) &= ~4;
    }
}
