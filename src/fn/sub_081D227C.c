#include "global.h"

#define TaggedSceneNpc_UpdateState17Animation sub_081D227C

void TaggedSceneNpc_PlayTaggedAnimation(void *, int, int, int, int);
int DisplayAnimation_TestFinished(void *);
int Anim_Step(void *, void *);
s32 Mod(s32, s32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);

void TaggedSceneNpc_UpdateState17Animation(void *unused, u8 *p, s32 tick)
{
    u8 *tag = p + 0x20;
    if (p[9]) {
        s32 zero = 0;
        p[9] = zero;
        p[7] = zero;
        TaggedSceneNpc_PlayTaggedAnimation(p, 5, 2, 0, 4);
        *(u16 *)(p + 0x114) = 3;
        *(u16 *)(p + 0x10c) = zero;
        Anim_Step(p + 0x10c - 0xc, p + 0xe0);
    }
    {
        u32 mask = 0x800;
        *(u16 *)(p + 0xe) |= mask;
    }
    if (DisplayAnimation_TestFinished(p + 0xd4))
        p[7] = 1;
    if (*(u16 *)(tag + 0x16) != 0 && tick > 0) {
        s32 r = Mod(tick, 70);
        if (r == 0)
            *(void **)(tag + 0x18) = ActorAttachment_UpdateFromDescriptor(
                *(void **)(tag + 0x18), p + 0x5c, 2, 0, (void *)r, r, 0x80, r);
    }
}
