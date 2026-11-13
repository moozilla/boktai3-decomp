#ifndef GUARD_TILE_ANIMATION_H
#define GUARD_TILE_ANIMATION_H

#include "engine.h"

struct TileAnimationChannel {
    u16 id, elapsed;          /* 0x00 */
    u8 active, tileCount;     /* 0x04 */
    u8 frame, frameCount;     /* 0x06 */
    u8 ignorePause, dirty;    /* 0x08 */
    u8 removePending, unused; /* 0x0A */
    u16 source[8];            /* 0x0C: source tile indices */
    u16 destination[8];       /* 0x1C: VRAM tile indices */
    u16 duration[8];          /* 0x2C: frame durations */
};

struct TileAnimation {
    struct Task task;                        /* 0x00 */
    u16 tileCount, unused;                   /* 0x18 */
    struct TileAnimationChannel channels[8]; /* 0x1C */
    void *tiles;                             /* 0x1FC: source tile buffer */
};

struct TileAnimation *TileAnimation_GetTask(void);
void TileAnimation_Advance(struct TileAnimation *, struct TileAnimationChannel *);
s32 TileAnimation_Update(struct TileAnimation *);
u32 TileAnimation_Destroy(struct TileAnimation *);
s32 TileAnimation_Initialize(struct TileAnimation *);
struct TileAnimation *TileAnimation_Create(void);
void TileAnimation_Upload(void);
struct TileAnimationChannel *TileAnimation_FindFreeChannel(struct TileAnimation *);
struct TileAnimationChannel *TileAnimation_FindChannel(struct TileAnimation *, u32);
void TileAnimation_Configure(void);
void TileAnimation_Stop(void);

s32 Bg_CopyTiles(void *resource, s32 mode, void *destination, s32 firstTile, s32 tileCount);

#endif
