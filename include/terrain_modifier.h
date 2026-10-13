#ifndef GUARD_TERRAIN_MODIFIER_H
#define GUARD_TERRAIN_MODIFIER_H

#include "global.h"

/* Tentative: a modifier applied to a map cell (tile/height override). */
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

struct TerrainMap {
    u8 pad[36];
    u16 rows[1];
};

/* Script-read position (three 16-bit values, 6 bytes). */
struct TerrainScriptVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};

extern struct TerrainModifierManager *gUnk_02000028;
extern struct TerrainMap *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;

void sub_0821D698(void *, s32, s32, s32, s32, s32);
s32 sub_0821ABA8(s32, s32);
void sub_0821A04C(void *, u32 (*)(void), u32 (*)(void));

u32 sub_08005E1C(struct TerrainModifier *);
u32 sub_08005E40(struct TerrainModifier *);
s32 TerrainModifiers_Create(s32 id, struct TerrainVec *position, s32 kind, s32 tile, s32 flags);
u32 sub_08005F40(struct TerrainModifier *);
s32 sub_08005F78(void);
s32 sub_08005FA8(u16 id);
s32 sub_08005FE4(u16 id);
s32 sub_0800602C(u32 id);
s32 TerrainModifiers_Change(s32 id, s32 tile, s32 kind, s32 flags, s32 cell, s32 height);
u32 sub_0800611C(void);
u32 sub_08006120(void);
s32 sub_08006134(struct TerrainModifierManager *manager, s32 unused);
void *sub_08006144(s32 a, s32 unused);

#endif
