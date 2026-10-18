#include "global.h"
struct SceneAnimation {
    u8 unknown0[0x10];
    u16 state;
    u8 unknown12[0x36];
    u8 animation[0x10];
    void *data;
    u32 parameter;
    u8 unknown60[0xa4];
    u16 active;
};
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void SceneAnimation_SelectSequence(struct SceneAnimation *p, u32 sequence, u32 flags, u32 parameter)
{
    if (sequence == 3) {
        u16 state = 7;
        u16 inactive = 0;
        p->state = state;
        p->active = inactive;
    } else {
        Anim_SetSequence(p->animation, p->data, (u16)sequence, 0, (u8)flags);
        p->active = 1;
        p->parameter = parameter;
    }
}
