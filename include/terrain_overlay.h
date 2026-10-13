#ifndef TERRAIN_OVERLAY_H
#define TERRAIN_OVERLAY_H
#include "engine.h"

// Tentative: terrain-edge overlays drawn around tracked actors.
struct TerrainOverlayDrawable {
    u32 flags; // bit 0: hidden
    u8 active;
    u8 pad5[11];
    u16 tile;
    u8 pad12[6];
    u32 drawFlags;
    s16 x, y, z;
    u8 tail[10];
};

struct TerrainOverlayActorFlags {
    u32 unused[2], flags;
};

struct TerrainOverlayActor {
    u32 unused, flags;
    u8 pad8[4];
    s16 x, y, z;
    u8 pad12[26];
    u32 *state;
    struct TerrainOverlayActorFlags *parent;
};

// Tracked actor slot
struct TerrainOverlayEntry {
    u16 used, id;
    struct TerrainOverlayActor *actor;
};

struct TerrainOverlay {
    u8 task[0x18]; // task header
    u16 id, resource;
    s32 initialized, actorCount, drawableCount;
    u32 count; // queued commands
    u16 commands
        [16][2]; // {type, id}; type 0 = track actor, 1 = untrack (a struct pair changes codegen)
    u8 model[28];
    struct TerrainOverlayDrawable *drawables;
    u32 *keys; // dedupe keys, one per drawable
    struct TerrainOverlayEntry entries[1];
};

struct TerrainOverlayCell {
    u8 height, tile;
    u16 flags;
};

#endif
