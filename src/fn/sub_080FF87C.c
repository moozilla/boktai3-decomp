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
static inline u32 Step(struct WoodenCrate *spriteInput, struct DisplaySequenceState *a,
                       struct WoodenCrate *crate)
{

    register struct WoodenCrate *sprite asm("r8") = spriteInput;
    const volatile u16 *entry;
    u32 done;
    {
        u32 offset = a->index * 2;
        const u16 *table = crate->animation.commands;
        entry = (const volatile u16 *)((u32)table + offset);
    }
    {
        u32 frame = *entry >> 6;
        struct WoodenCrate *base = sprite;
        base->frame = frame;
    }
    {
        u32 input = a->flags;
        u32 mask = 1;
        u32 left;
        asm("" : "+r"(input), "+r"(mask));
        left = mask & input;
        asm("" : "+r"(left));
        {
            register u32 flags asm("r0");
            if (left != (((*entry & 0x30) >> 4) & mask)) {
                register struct WoodenCrate *base asm("r1") = sprite;
                flags = base->flags;
                flags |= 4;
            } else {
                register struct WoodenCrate *base asm("r1") = sprite;
                flags = base->flags;
                flags &= ~4;
            }
            {
                struct WoodenCrate *base = sprite;
                base->flags = flags;
            }
        }
    }
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 2;
        register u32 left asm("r1");
        left = mask & input;
        left = (u8)left;
        {
            register u32 flags asm("r0");
            if (left != (((*entry & 0x30) >> 4) & mask)) {
                register struct WoodenCrate *base asm("r1") = sprite;
                flags = base->flags;
                flags |= 8;
            } else {
                register struct WoodenCrate *base asm("r1") = sprite;
                flags = base->flags;
                flags &= ~8;
            }
            {
                struct WoodenCrate *base = sprite;
                base->flags = flags;
            }
        }
    }
    {
        u32 elapsed = a->elapsed + 1;
        register u32 mask asm("r5");
        register u32 temp asm("r1");
        u32 duration;
        register u32 savedDuration asm("r3");
        u32 flags;
        a->elapsed = elapsed;
        temp = 65535;
        asm("" : "+r"(temp));
        mask = temp;
        duration = a->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            a->elapsed = 0;
            flags = a->flags;
            if (flags & 4) {
                u32 index = a->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold)
                        a->elapsed = savedDuration;
                    else
                        a->index = a->frameCount - 1;
                    done = 1;
                } else {
                    a->index = index - 1;
                    done = 0;
                }
            } else {
                u32 index = a->index + 1;
                u32 frameCount;
                a->index = index;
                frameCount = a->frameCount;
                if ((index & mask) >= frameCount) {
                    u32 hold = 8;
                    hold &= flags;
                    if ((u8)hold) {
                        a->elapsed = duration;
                        a->index = frameCount - 1;
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
    }
    return done;
}
// confidence=low: allocates a crate slot and reads sprite, trigger and callback options from the script.
void WoodenCrate_CreateFromScript(void)
{
    struct WoodenCrateManager *manager = gWoodenCrateManager;
    struct WoodenCrate *crate;
    register struct WoodenCrate *sprite asm("r8");
    if (!manager) {
        manager = WoodenCrate_GetOrCreateManager();
        if (!manager)
            return;
    }
    crate = WoodenCrate_AllocateSlot(manager);
    if (!crate)
        return;
    sprite = crate;
    AnimData_Load(crate->resource, 0xBB68);
    Sprite_Init(crate, crate->resource, 0);
    crate->state = 1;
    crate->changed = 1;
    if (Script_SeekToKeyword('p')) {
        crate->position.x = Script_GetValue();
        crate->position.y = Script_GetValue();
        crate->position.z = Script_GetValue();
    }
    crate->origin = crate->position;
    crate->priority = Script_GetKeywordValue('R', 2);
    crate->sequenceFlags = Script_GetKeywordValue('r', 0);
    crate->name = Script_GetKeywordValue('n', 0);
    crate->toggleScript = Script_GetKeywordValue('C', 0);
    crate->releaseScript = Script_GetKeywordValue('D', 0);
    crate->crateFlags = Script_GetKeywordValue('s', 1);
    crate->palette = Script_GetKeywordValue('k', 1);
    crate->triggerTime = Script_GetKeywordValue('t', 0);
    if (crate->triggerTime)
        WoodenCrate_SetFlags(crate, 4);
    switch (crate->palette) {
        case 0:
            AnimData_SelectPaletteBank(crate->drawable, 0x1A1);
            break;
        case 1:
            AnimData_SelectPaletteBank(crate->drawable, 0x1A0);
            break;
    }
    if (WoodenCrate_HasFlags(crate, 1))
        Anim_SetSequence(&crate->animation, manager->animationData, 0, (u8)crate->palette,
                         crate->sequenceFlags);
    else
        Anim_SetSequence(&crate->animation, manager->animationData, 2, (u8)crate->palette,
                         crate->sequenceFlags);
    crate->originalFlags = crate->crateFlags;
    Step(sprite, &crate->animation, crate);
    WoodenCrate_InitCollider(crate);
}
