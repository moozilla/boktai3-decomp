#include "global.h"
#include "engine.h"

// foundry draft (worker best 99.1%); what differs is mostly register allocation and instruction order
#define gActorStateCallbacks gUnk_0824EF10

struct ActorAnimationStatus {
    u32 flags : 8;
};
struct ActorControllerAnimation {
    u32 unk0, unk4, flags;
};
extern u32 gUnk_030053F0, gUnk_02000560;
extern void *gUnk_0824EF10[];

static inline u8 TestFlag(u8 *p, u32 mask)
{
    return *p & mask;
}

void ActorController_UpdateAnimation(u8 *p)
{
    u8 *finished;
    struct ActorControllerAnimation *anim;
    u32 mask = 0x400;
    if (!((gEngineFlags | gUnk_030053F0) & mask)) {
        u32 four = 4;
        if (!(gUnk_02000560 & four))
            Callback_InvokeActor(p, gUnk_0824EF10[p[0xB3F]]);
    }
    finished = p + 0xB43;
    *finished = 0;
    anim = (void *)(p + 0xB4);
    if (Anim_Step(anim, p + 0x94))
        *finished = 1;
    {
        u32 one;
        u8 *q = p + 0xD1;
        u32 r = *(volatile u8 *)q;
        one = 1;
        if (one & r)
            *finished = one;
        if (*(u16 *)(p + 0xB32) & 0x800)
            anim->flags |= one;
        else
            anim->flags &= ~1;
    }
    if (*(u16 *)(p + 0xB32) & 0x400)
        anim->flags |= 0x200;
    else
        anim->flags &= ~0x200;
    p[0xB42]++;
}
