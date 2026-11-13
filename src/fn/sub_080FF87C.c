#include "global.h"
#include "display_sequence_sprite.h"
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
// confidence=low: allocates a crate slot and reads sprite, trigger and callback options from the script.
void WoodenCrate_CreateFromScript(void)
{
    struct WoodenCrateManager *manager = gWoodenCrateManager;
    struct WoodenCrate *crate;
    struct WoodenCrate *sprite;
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
    DisplaySequence_StepSprite((struct DisplaySequenceSprite *)sprite, &crate->animation);
    WoodenCrate_InitCollider(crate);
}
