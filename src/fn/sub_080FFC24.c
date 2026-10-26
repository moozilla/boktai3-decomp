// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "contact_actor.h"
#include "ram.h"
struct WoodenCrate {
    u32 flags;
    u8 pad4[3];
    u8 priority;
    u8 pad8[4];
    void *drawable;
    u16 frame;
    u8 pad12[10];
    struct ContactPosition position;
    u8 pad24[8];
    struct DisplaySequenceState animation;
    u8 resource[0x1C];
    struct ContactPosition origin;
    u8 pad60[0x54];
    u32 name, toggleScript, releaseScript;
    u8 padC0[4];
    u16 state, index, crateFlags;
    s16 palette;
    u8 sequenceFlags, flashTimer;
    u16 originalFlags, triggerTime, triggerCount;
    u8 changed;
};
struct WoodenCrateManager {
    u8 pad[0x18];
    void *animationData;
};
struct WoodenCrateManager *WoodenCrate_GetOrCreateManager(void);
struct WoodenCrate *WoodenCrate_AllocateSlot(struct WoodenCrateManager *);
void WoodenCrate_InitCollider(struct WoodenCrate *);
void WoodenCrate_UpdatePaletteTimers(struct WoodenCrate *);
void WoodenCrate_SetFlags(struct WoodenCrate *, u16);
u8 WoodenCrate_HasFlags(struct WoodenCrate *, s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 Script_GetKeywordValue(s32, s32);
struct CrateArguments {
    u32 count : 16;
    u32 pad : 16;
    s32 *values;
};
static inline u32 Step(struct WoodenCrate *crate)
{
    struct DisplaySequenceState *a = &crate->animation;
    const volatile u16 *entry;
    u32 done;
    {
        u32 offset = a->index * 2;
        const u16 *table = crate->animation.commands;
        entry = (const volatile u16 *)((u32)table + offset);
    }
    crate->frame = *entry >> 6;
    {
        u32 input = a->flags;
        u32 mask = 1;
        u32 left;
        asm("" : "+r"(input), "+r"(mask));
        left = mask & input;
        asm("" : "+r"(left));
        if (left != (((*entry & 0x30) >> 4) & mask))
            crate->flags |= 4;
        else
            crate->flags &= ~4;
    }
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 2;
        register u32 left asm("r1");
        left = mask & input;
        left = (u8)left;
        if (left != (((*entry & 0x30) >> 4) & mask))
            crate->flags |= 8;
        else
            crate->flags &= ~8;
    }
    if (++a->elapsed >= a->duration) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->index == 0) {
                if (a->flags & 8)
                    a->elapsed = a->duration;
                else
                    a->index = a->frameCount - 1;
                done = 1;
            } else {
                --a->index;
                done = 0;
            }
        } else {
            if (++a->index >= a->frameCount) {
                if ((u8)(a->flags & 8)) {
                    a->elapsed = a->duration;
                    a->index = a->frameCount - 1;
                } else
                    a->index = 0;
                done = 1;
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        a->frameWeight = *entry & 15;
        a->duration = (a->frameWeight * a->speed) >> 6;
        if (!a->duration)
            a->duration = 1;
    } else
        done = 0;
    return done;
}
// confidence=low: finishes the opening/closing animation and invokes its script with crate name and flags.
void WoodenCrate_UpdateToggleAnimation(struct WoodenCrateManager *manager,
                                       struct WoodenCrate *crate)
{
    WoodenCrate_UpdatePaletteTimers(crate);
    if (Step(crate)) {
        s32 values[2];
        struct CrateArguments args;
        if (crate->animation.flags & 4) {
            Anim_SetSequence(&crate->animation, manager->animationData, 0, (u8)crate->palette,
                             crate->sequenceFlags);
            WoodenCrate_SetFlags(crate, 1);
        } else {
            Anim_SetSequence(&crate->animation, manager->animationData, 2, (u8)crate->palette,
                             crate->sequenceFlags);
            WoodenCrate_SetFlags(crate, 2);
        }
        crate->state = 1;
        crate->changed = 1;
        values[0] = crate->name;
        values[1] = crate->crateFlags;
        args.values = values;
        args.count = 2;
        if (crate->toggleScript)
            Script_Call(crate->toggleScript, &args);
    }
}
