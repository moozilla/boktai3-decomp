#ifndef GUARD_FORGING_BOOK_H
#define GUARD_FORGING_BOOK_H

#include "global.h"
#include "engine/engine.h"
#include "engine/task.h"
#include "engine/video/anim_sprite.h"
#include "engine/video/sprite.h"
#include "scene/collision/contact_swarm.h"
#include "system/game_state/game_state.h"

/* Names are provisional. */
struct ForgingBook {
    struct Task task;
    void *backgroundMap;
    u32 unknown1C;
    u32 frame; /* frames since the category preview last changed */
    struct AnimSpriteResource resource, cursorResource;
    struct AnimSprite background, cursor, completionBadge, categoryHeading;
    struct AnimSprite countDigits[4];
    u16 cursorColors[16];
    s16 pulseFrame;
    u16 unknown386;
    struct AnimSpriteResource cellResource;
    struct AnimSprite cells[18];
    struct AnimSpriteResource previewResource;
    struct AnimSprite preview;
    u8 animationBase, animationFlag;
    s16 animationIndex;
    u16 previewColors[16];
    s16 previewStep;
    u16 unknownB0E;
    struct DisplaySequenceSprite sprite;
    struct ContactSwarmGraphics spriteGraphics;
    void *debrisTask, *attackEffectTask;
    s16 category;
    u16 unknownB62;
    s16 column, row;
    s16 selectedEntry;
    u16 unknownB6A;
    s16 exiting, fadeShift, fadeTimer;
    u16 unknownB72;
    s32 textWindow;
    const void *nameScript, *helpScript, *itemNameScript;
    u8 entries[0x61];
    u8 emissionMode;
    u8 unknownBE6, unknownBE7;
    u32 exitScript;
    u8 emptyString[4];
};

typedef char ForgingBookSizeCheck[sizeof(struct ForgingBook) == 0xBF0 ? 1 : -1];

s32 ForgingBook_GetCategoryEntryCount(s32 category);
u32 ForgingBook_GetEntryAnimation(u32 entry);
s32 ForgingBook_GetCategoryEntryOffset(s32 category);
s32 ForgingBook_CountDiscoveredCategoryEntries(struct ForgingBook *book, s32 category);
u32 ForgingBook_IsCategoryComplete(struct ForgingBook *book, s32 category);
u32 ForgingBook_HasDiscoveredAllWeapons(void);
u32 ForgingBook_HasDiscoveredAllItems(void);
u32 ForgingBook_IsWeaponDiscovered(s32 weapon);
u32 ForgingBook_IsItemDiscoveryRecorded(s32 item);
u32 ForgingBook_IsItemDiscovered(s32 item);

#endif
