#ifndef GUARD_COLLECTION_BOOK_H
#define GUARD_COLLECTION_BOOK_H

#include "global.h"
#include "engine/engine.h"
#include "engine/task.h"
#include "engine/video/anim_sprite.h"
#include "engine/video/sprite.h"
#include "scene/collision/contact_swarm.h"
#include "system/game_state/game_state.h"

/* The ten-entry collection grid. Names are provisional. */
struct CollectionBook {
    struct Task task;
    void *backgroundMap;
    u32 unknown1C;
    u32 unknown20;
    struct AnimSpriteResource resource, markerResource;
    struct AnimSprite background, cursor, completionBadge, pageHeading;
    struct AnimSprite countDigits[4];
    struct AnimSprite trackedMarker;
    u16 cursorColors[16];
    s16 pulseFrame;
    u16 unknown3E6;
    struct ContactSwarmGraphics graphics[10];
    struct DisplaySequenceSprite entries[10];
    u8 unknown6B8[0x28];
    s16 entryIds[10];
    s16 page, unknown6F6, column, row;
    s16 selectedEntry;
    u16 unknown6FE;
    s16 exiting, fadeShift, fadeTimer;
    s16 trackingLocked;
    s32 textWindow;
    const void *nameScript, *descriptionScript, *lockedDescriptionScript;
    u32 exitScript;
    s16 closing, busy;
    u16 unknown720;
    s16 trackedEntry;
};

typedef char CollectionBookSizeCheck[sizeof(struct CollectionBook) == 0x724 ? 1 : -1];

u32 CollectionBook_GetPageEntryCount(u32 page);
s32 CollectionBook_IsEntryCollected(s32 entry);
s32 CollectionBook_CountCollectedEntries(void);
u32 CollectionBook_IsComplete(void);

#endif
