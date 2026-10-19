#include "global.h"
#include "state_actor.h"

#define StateActor_StartKind5Animation9AndAttach sub_081CA2C8

struct Context {
    u8 pad[0x434];
    struct StateActorPos *target;
    struct StateActor *targetActor;
};

void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, u32, u32, u32, u32);
s32 sub_081C8FB0(struct Context *, struct StateActor *, s32);

void sub_081CA2C8(struct Context *ctx, struct StateActor *p, u32 a2)
{
    u32 r;
    u32 mask;

    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4: r = 0; break;
            case 5: r = 9; break;
            case 6: r = 0; break;
            case 7: r = 0; break;
            case 8: r = 0; break;
            case 9: r = 0; break;
            case 10: r = 0; break;
            case 11: r = 0; break;
            case 12: r = 0; break;
            case 13: r = 0; break;
            case 14: r = 0; break;
            case 15: r = 0; break;
            case 16: r = 0; break;
            case 17: r = 0; break;
            case 18: r = 0; break;
            case 19: r = 0; break;
            case 20: r = 0; break;
            case 21: r = 0; break;
            case 22: r = 0; break;
            case 23: r = 0; break;
            case 24: r = 0; break;
            case 25: r = 0; break;
            case 26: r = 0; break;
            case 27: r = 0; break;
            case 28: r = 0; break;
            case 29: r = 0; break;
            default: r = 0; break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 1, 4);
    }

    if (sub_081C8FB0(ctx, p, 0)) {
        u16 m = 0xa0 << 6;
        p->displayFlags |= m;
    }
    {
        u16 m = 0x80 << 5;
        p->displayFlags |= m;
    }

    mask = a2 & 0x3f;
    if (mask == 0) {
        void **slot = (void **)((u8 *)p + 0x2c0);
        *slot = ActorAttachment_UpdateFromDescriptor(*slot, (u8 *)p + 0x5c, 2, 0, mask, mask, 0xfa, 0xfa);
    }

    if (DisplayAnimation_TestFinished((u8 *)p + 0xd4))
        p->done = 1;
}
