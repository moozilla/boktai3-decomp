#ifndef GUARD_ANIMATED_TILE_H
#define GUARD_ANIMATED_TILE_H
#include "display_animation.h"
struct AnimatedTile {
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState animation;
    struct SceneAnimation graphics;
    s32 name;
    u32 script;
    u16 state;
    s16 slot;
    u8 sequence, entered;
    u16 timer;
};
struct AnimatedTileManager {
    struct Task task;
    void *sequences;
    struct AnimatedTile tiles[3];
    u32 used;
};
struct AnimatedTile *AnimatedTile_Allocate(struct AnimatedTileManager *);
struct AnimatedTile *Pool1BC_FindByName(u32);
struct AnimatedTileManager *AnimatedTile_CreateManager(void);
s32 AnimatedTile_InitManager(struct AnimatedTileManager *);
s32 CallbackSlots_DispatchActive(struct AnimatedTileManager *);
u32 AnimatedTile_OnManagerDestroy(struct AnimatedTileManager *);
#endif
