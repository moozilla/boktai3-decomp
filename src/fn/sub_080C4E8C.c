#include "global.h"
#include "player.h"

#define Skeleton_TryTriggerAction sub_080C4E8C

void sub_080C4CFC(void *s);

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

struct SkeletonLink {
    u8 filler[0x628];
    u32 h628;
};

struct SkeletonActor {
    u8 filler0[0xDC];
    u32 h0DC;
    u8 filler1[0x2D4 - 0xE0];
    u16 flags;
    u8 filler2[0x3D0 - 0x2D6];
    struct SkeletonLink *t;
};

void sub_080C4E8C(struct SkeletonActor *s)
{
    struct SkeletonLink *t = s->t;
    u32 mask;

    mask = 0x80000;
    if (Test(t->h628, mask))
        return;

    mask = 0x180;
    if (!(mask & s->flags))
        return;

    {
        struct Player *player = gUnk_02000580[0];
        mask = 0x20000;
        if (player->statusFlags & mask)
            return;
    }

    if (s->h0DC == 0)
        return;

    sub_080C4CFC(s);
}
