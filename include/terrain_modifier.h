#ifndef GUARD_TERRAIN_MODIFIER_H
#define GUARD_TERRAIN_MODIFIER_H

#include "scene_vec.h"
#include "terrain_collision.h"

struct TerrainModifier {
    u16 id;
    u8 active; // registration is live
    u8 kind;
    u8 shape;
    u8 height;
    u16 flags;
    u16 cell;
    u8 padA[2];
    struct TerrainCellOverride registration;
    struct TerrainModifier *previous;
    struct TerrainModifier *next;
};

struct TerrainModifierManager {
    struct Task task;
    struct TerrainModifier *head;
};

/* 0x02000028 */
extern struct TerrainModifierManager *gTerrainModifierManager;

u32 TerrainModifiers_RegisterCell(struct TerrainCellOverride *, u32, u32, u32, u32, u32);

u32 TerrainModifiers_AddToList(struct TerrainModifier *);
u32 TerrainModifiers_RemoveFromList(struct TerrainModifier *);
s32 TerrainModifiers_Create(s32 id, struct SceneVec *position, s32 kind, s32 shape, s32 flags);
u32 TerrainModifiers_Destroy(struct TerrainModifier *);
s32 TerrainModifiers_DestroyAll(void);
s32 TerrainModifiers_DestroyById(u16 id);
s32 TerrainModifiers_Deactivate(u16 id);
s32 TerrainModifiers_Activate(u32 id);
s32 TerrainModifiers_Change(s32 id, s32 shape, s32 kind, s32 flags, s32 cell, s32 height);
u32 TerrainModifierManager_Update(void);
u32 TerrainModifierManager_Destroy(void);
s32 TerrainModifierManager_Init(struct TerrainModifierManager *manager, s32 unused);
void *TerrainModifierManager_Create(s32 a, s32 unused);

#endif
