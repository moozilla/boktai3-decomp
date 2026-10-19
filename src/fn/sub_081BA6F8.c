#define Registry3F74_UpdateAnimations sub_081BA6F8
#define Registry3F74_FindFrameOffset sub_081BB178
#include "global.h"

struct Registry3F74Resource { u8 unknown[0x1A4]; u32 flags; };
struct Registry3F74Animation { u8 unknown[8]; u32 flags; u8 unknownC[0x1C]; u16 offsetX, offsetY; u8 tail[0x34]; };
struct Registry3F74Object {
    u8 unknown[0x80];
    struct Registry3F74Animation animations[4];
    u8 data[0x24];
    s32 frame;
    s32 selection;
    struct Registry3F74Resource *resource;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 Anim_Step(void *, void *);
u16 *sub_081BB178(struct Registry3F74Resource *, s32);
static inline u32 TestFlags(struct Registry3F74Resource *p, u32 mask) { return p->flags & mask; }
void sub_081BA6F8(struct Registry3F74Object *p)
{
    void *data = p->data;
    struct Registry3F74Animation *anim = &p->animations[0];
    u16 *offset;
    if (p->selection >= 0 && TestFlags(p->resource, 2)) {
        anim->flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(p->selection + 0x2B), 0);
    } else {
        anim->flags |= 1;
    }
    anim = &p->animations[2];
    offset = sub_081BB178(p->resource, p->frame);
    if (offset) {
        anim->offsetX = offset[0];
        anim->offsetY = offset[1];
    } else {
        anim->offsetX = 0;
        anim->offsetY = 0;
    }
    anim = &p->animations[3];
    if (p->selection == 1)
        Anim_Step(anim, data);
}
