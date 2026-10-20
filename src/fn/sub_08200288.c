#include "global.h"

#define Anim_SetFrame Anim_SetFrameDurationScale

struct PlayerClone {
    u8 unknown000[0x1b8];
    struct {
        u8 unknown00[8];
        u32 flags;
        u8 unknown0c[0xb4];
    } anim;
    u8 resource[0x20];
    u8 unknown298[0x84];
    u32 animation;
    u8 animationOffset;
    u8 unknown321;
    u8 mirrored;
};
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void Anim_SetFrameDurationScale(void *, u32);

s32 PlayerClone_SetAnimation(struct PlayerClone *s, s32 index, u32 frame, u32 mode)
{
    u32 same = 0;
    if (s->animation == index)
        same = 1;
    s->animation = index;
    Anim_SetKeyframes(&s->anim, s->resource, (u16)((u16)(*(&index)) + s->animationOffset),
                      (u16)mode, same);
    if (s->mirrored)
        s->anim.flags |= 4;
    else
        s->anim.flags &= ~4;
    Anim_SetFrameDurationScale(&s->anim, (u16)frame);
    return Anim_Step(&s->anim, s->resource);
}
