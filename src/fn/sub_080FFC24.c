#include "global.h"
#include "display_animation.h"
#include "display_sprite_playback.h"
#include "contact_actor.h"
#include "ram.h"
struct WoodenCrate {
    struct ContactSwarmSprite sprite;
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
// confidence=low: finishes the opening/closing animation and invokes its script with crate name and flags.
void WoodenCrate_UpdateToggleAnimation(struct WoodenCrateManager *manager,
                                       struct WoodenCrate *crate)
{
    WoodenCrate_UpdatePaletteTimers(crate);
    if (PlayerCue_StepSequenceEnded(&crate->sprite, &crate->animation)) {
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
