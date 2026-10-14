// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
#define ScenePool_StateCruise sub_0803DDA8
struct PlayerFlags {
    u8 unknown0[0x8f4];
    u16 flags;
};
extern struct PlayerFlags *gUnk_020004F0;
void sub_0803DA7C(struct ScenePoolEntry *, s32, s32, s32);
s32 sub_0803D9D8(struct ScenePoolEntry *);
s32 sub_0803D8A8(struct ScenePoolActor *, struct ScenePoolEntry *);
s32 sub_0803D680(struct ScenePoolActor *, struct ScenePoolEntry *);
s32 sub_0803D700(struct ScenePoolActor *, struct ScenePoolEntry *);
s32 sub_0803D78C(struct ScenePoolActor *, struct ScenePoolEntry *);
s32 sub_0803D80C(struct ScenePoolActor *, struct ScenePoolEntry *);
static inline void Or(u32 mask, u32 *flags)
{
    *flags |= mask;
}
static inline u8 PlayerHasFlag(u32 mask)
{
    if (gUnk_020004F0 && (gUnk_020004F0->flags & mask))
        return 1;
    return 0;
}
s32 ScenePool_StateCruise(void *manager, struct ScenePoolActor *actor, struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    s32 metric;
    if (entry->stateEntered) {
        entry->stateEntered = 0;
        history[0] = 0;
        history[1] = 0;
        entry->turnStep = 0x200;
        {
            u32 *flags = &actor->motionFlags;
            register u32 mask asm("r1");
            mask = ~1;
            *flags &= mask;
        }
    }
    {
        s32 hi = 0x28000;
        metric = entry->direction;
        if (metric > hi)
            goto outside;
        {
            register s32 inside asm("r2") = 1;
            if (metric < -0x28000)
                inside = 0;
            if (!inside)
                goto outside;
        }
        {
            if (metric > 0x1e000)
                actor->controlFlags |= 0x10;
            else if (metric < -0x1e000)
                actor->controlFlags |= 0x20;
            history[0]++;
            history[1]--;
            goto rangeDone;
        }
    outside: {
        if (metric > 0x78000)
            actor->controlFlags |= 0x300;
        else if (metric < -0x14000)
            actor->controlFlags |= 0x20;
        else
            actor->controlFlags |= 0x110;
        history[0] = 0;
        history[1]++;
    }
    rangeDone:;
    }
    {
        const u16 *table = gUnk_0203B400;
        u32 *index = &gUnk_03005308;
        u32 next = *index + 1;
        u32 indexMask = 0x3ff;
        register u32 byteMask asm("r6");
        const u8 *randomAddress;
        s32 random;
        asm("" : "+r"(table), "+r"(index));
        next &= indexMask;
        *index = next;
        randomAddress = (const u8 *)((next << 1) + (u32)table);
        byteMask = 255;
        random = *randomAddress;
        if (random <= 31) {
            s32 z = actor->position[2] >> 12;
            if (z <= 18)
                sub_0803DA7C(entry, 1, 3, 6);
            else if (z > 47)
                sub_0803DA7C(entry, -1, 3, 6);
            else {
                u32 value;
                u32 second = (next + 1) & indexMask;
                *index = second;
                value = table[second];
                random = value & byteMask;
                if (random <= 127)
                    sub_0803DA7C(entry, 1, 3, 6);
                else
                    sub_0803DA7C(entry, -1, 3, 6);
            }
        }
    }
    if (sub_0803D9D8(entry))
        goto done;
    if (sub_0803D8A8(actor, entry))
        goto done;
    Or(0x10, &actor->stateFlags);
    if (!PlayerHasFlag(2)) {
        switch (entry->kind) {
            case 0:
                sub_0803D680(actor, entry);
                break;
            case 1:
                sub_0803D700(actor, entry);
                break;
            case 2:
                sub_0803D78C(actor, entry);
                break;
            case 3:
                sub_0803D80C(actor, entry);
                break;
        }
    }
    entry->stateFrame++;
done:
    return 0;
}
