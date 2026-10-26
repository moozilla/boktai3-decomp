#ifndef GUARD_TERRAIN_MODIFIER_H
#define GUARD_TERRAIN_MODIFIER_H

#include "engine.h"

struct TerrainModifier {
    u16 id;
    u8 active; // registration is live
    u8 kind;
    u8 tile;
    u8 height;
    u16 flags;
    u16 cell;
    u8 padA[2];
    u8 registration[16];
    struct TerrainModifier *previous;
    struct TerrainModifier *next;
};

struct TerrainModifierManager {
    u8 pad[24];
    struct TerrainModifier *head;
};

struct TerrainVec {
    s16 x, y, z, pad;
};

/* Script-read position (three 16-bit values, 6 bytes). */
struct TerrainScriptVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};

extern struct TerrainModifierManager *gTerrainModifierManager;

void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);

u32 TerrainModifiers_AddToList(struct TerrainModifier *);
u32 TerrainModifiers_RemoveFromList(struct TerrainModifier *);
s32 TerrainModifiers_Create(s32 id, struct TerrainVec *position, s32 kind, s32 tile, s32 flags);
u32 TerrainModifiers_Destroy(struct TerrainModifier *);
s32 TerrainModifiers_DestroyAll(void);
s32 TerrainModifiers_DestroyById(u16 id);
s32 TerrainModifiers_Deactivate(u16 id);
s32 TerrainModifiers_Activate(u32 id);
s32 TerrainModifiers_Change(s32 id, s32 tile, s32 kind, s32 flags, s32 cell, s32 height);
u32 TerrainModifierManager_Update(void);
u32 TerrainModifierManager_Destroy(void);
s32 TerrainModifierManager_Init(struct TerrainModifierManager *manager, s32 unused);
void *TerrainModifierManager_Create(s32 a, s32 unused);

#endif
