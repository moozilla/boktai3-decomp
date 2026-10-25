#include "global.h"

/* map cell index under a packed (x, y) position, or 0 when outside the map */

/* terrain height of a cell, adjusted by the slope bytes of pos */

/* body shared by the state-entry path and the flagged path */

// confidence=low: timed wander/turn state of a ground enemy: picks a heading, probes terrain heights and mesh, queues state switch
#define B9158_AT(T, off) (*(T *)((u8 *)a + (off)))

extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern u32 gUnk_030053F4;
extern u8 *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
extern u8 *gUnknown_02000580[];
extern const s16 gUnk_086149C4[];

s32 SceneActorTerrain_FindOpenNeighborDirection(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void *TerrainModifiers_FindCellOverride(s32, s32);
void Callback_InvokeActor(void *, void *);
s32 Nav_IsPositionInMesh(void *, void *, void *);
s32 Terrain_ApplyBlocked(void *, void *, void *);
void sub_0807E334(void *);
void Memory_Clear(void *, u32);
s32 Mod(s32, s32);
s32 Div(s32, s32);

/* map cell index under a packed (x, y) position, or 0 when outside the map */
static inline s32 b9158_cell(u16 px, u16 py)
{
    s32 x = (s8)(px >> 8);
    s32 y = (s8)(py >> 8);

    if (x >= 0 && y >= 0 && (u32)x < gUnk_030052F8 && (u32)y < gUnk_030052FC)
        return *(u16 *)(gUnk_030052F4 + 0x24 + y * 2) + x;
    return 0;
}

/* terrain height of a cell, adjusted by the slope bytes of pos */
static inline s32 b9158_height(s32 cell, u8 *pos)
{
    u8 *cp = TerrainModifiers_FindCellOverride(cell, 1);
    s32 h;
    s32 kind;

    if (cp != 0)
        cp += 4;
    else
        cp = *(u8 **)(gUnk_030052F4 + 4) + (cell << 2) + 0xC;
    kind = *cp >> 4;
    h = (*cp & 0xF) << 8;
    if (kind == 1)
        h -= pos[4];
    else if (kind == 2)
        h -= pos[0];
    return h;
}

/* body shared by the state-entry path and the flagged path */
static inline void b9158_Enter(u8 *a)
{
    u8 *ctx = a + 0x3D4;
    u8 *pos;
    u8 *node;
    s32 cell;
    s32 idx;
    s32 i;
    s32 cur;
    s32 rnd;
    s32 saved;
    s32 mesh;
    s32 r;
    u32 *copy;
    u16 st[3];
    u8 *src;

    B9158_AT(u8, 0x2A8) = 0;
    B9158_AT(u8, 0x2A9) = 0;
    B9158_AT(u8, 0x2AA) = 0xC;
    B9158_AT(u32, 0x2AC) = 0;
    B9158_AT(u8, 0x2B1) = 1;
    src = *(u8 **)(ctx + 0x48);
    B9158_AT(u8, 0x2B2) = 1;
    B9158_AT(u8, 0x2B0) = 0;
    B9158_AT(u8, 0x2B5) = 0x1A;
    B9158_AT(u32, 0x21C) = (u32)src;
    B9158_AT(u16, 0x2C8) = 0;
    B9158_AT(u32, 0x114) &= ~1;
    B9158_AT(u32, 0x104) = 0;

    pos = B9158_AT(u8 *, 0x5D4);
    node = *(u8 **)(B9158_AT(u8 *, 0x450));
    *(u16 *)pos = *(u16 *)node;
    *(u16 *)(pos + 4) = *(u16 *)(node + 2);
    *(u16 *)(pos + 2) = b9158_height(b9158_cell(*(u16 *)pos, *(u16 *)(pos + 4)), pos);

    if (gUnk_030053F4 & 0x4800) {
        u8 *slot = B9158_AT(u8 *, 0xDC);
        if (slot != 0) {
            u32 v = slot[8];
            idx = v > 3 ? -1 : (s32)v;
        } else {
            idx = -1;
        }
        if (idx < 0) {
            Callback_InvokeActor(a, *(void **)(a + 0x5F8));
            return;
        }
        cell = *(u16 *)(*(u8 **)(gUnknown_02000580[idx] + 0x40) + 6);
    } else {
        u8 *slot = B9158_AT(u8 *, 0xDC);
        cell = b9158_cell(*(u16 *)(slot + 0xC), *(u16 *)(slot + 0x10));
        idx = 0;
    }

    if (cell == B9158_AT(u16, 0x2F4) || (*(u32 *)(gUnknown_02000580[idx] + 0x20) & 0x20000)) {
        if (B9158_AT(u16, 0x2D4) & 0x400)
            return;
        B9158_AT(u16, 0x414) &= ~4;
        if (B9158_AT(u16, 0x2D4) & 0x80) {
            src = *(u8 **)(ctx + 0x48);
            B9158_AT(u8, 0x2B2) = 1;
            B9158_AT(u8, 0x2B0) = 0;
            B9158_AT(u8, 0x2B5) = 0x1A;
            B9158_AT(u32, 0x21C) = (u32)src;
            B9158_AT(u16, 0x2C8) = 0;
            B9158_AT(u32, 0x114) &= ~1;
            B9158_AT(u32, 0x104) = 0;
            B9158_AT(u8, 0x2A8) = 0;
            B9158_AT(u8, 0x2A9) = 0;
            B9158_AT(u8, 0x2AA) = 0xC;
            B9158_AT(u32, 0x2AC) = 0;
            B9158_AT(u8, 0x2B1) = 1;
        } else {
            B9158_AT(u8, 0x2B2) = 1;
            B9158_AT(u8, 0x2B0) = 0;
            B9158_AT(u8, 0x2B5) = 1;
            B9158_AT(u32, 0x21C) = B9158_AT(u32, 0x228);
            B9158_AT(u16, 0x2C8) = 0;
            B9158_AT(u32, 0x114) &= ~1;
            B9158_AT(u32, 0x104) = 0;
            B9158_AT(u8, 0x2A8) = 0;
            B9158_AT(u8, 0x2A9) = 0;
            B9158_AT(u8, 0x2AA) = 5;
            B9158_AT(u32, 0x2AC) = 0;
            B9158_AT(u8, 0x2B1) = 1;
            pos = B9158_AT(u8 *, 0x5D4);
            node = *(u8 **)(B9158_AT(u8 *, 0x450));
            *(u16 *)pos = *(u16 *)node;
            *(u16 *)(pos + 4) = *(u16 *)(node + 2);
            *(u16 *)(pos + 2) = b9158_height(b9158_cell(*(u16 *)pos, *(u16 *)(pos + 4)), pos);
            pos = B9158_AT(u8 *, 0x5D4);
            *(u16 *)(pos + 2) = b9158_height(b9158_cell(*(u16 *)pos, *(u16 *)(pos + 4)), pos);
            *(u16 *)(pos + 6) = 0;
        }

        saved = B9158_AT(u8, 0x59);
        if (B9158_AT(u16, 0x2D4) & 0x400) {
        } else if (B9158_AT(u16, 0x414) & 4) {
        } else {
            cur = B9158_AT(u8, 0x59);
            for (i = 0; i <= 3; i++) {
                gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
                rnd = (gUnk_0203B400[gUnk_03005308] & 3) << 6;
                if (rnd != cur)
                    break;
            }
            if (i > 3)
                rnd = cur;
            B9158_AT(u8, 0x5D8) = rnd;
            copy = (u32 *)(a + 0x460);
            copy[0] = B9158_AT(u32, 0x21C);
            a[0x467] = B9158_AT(u8, 0x2B5);
            a[0x464] = B9158_AT(u8, 0x2A8);
            a[0x465] = B9158_AT(u8, 0x2A9);
            a[0x466] = B9158_AT(u8, 0x2AA);
            B9158_AT(u8, 0x5D9) = saved;
            src = (u8 *)B9158_AT(u32, 0x5F0);
            B9158_AT(u8, 0x2B2) = 1;
            B9158_AT(u8, 0x2B0) = 0;
            B9158_AT(u8, 0x2B5) = 0x1B;
            B9158_AT(u32, 0x21C) = (u32)src;
            B9158_AT(u16, 0x2C8) = 0;
            B9158_AT(u32, 0x114) &= ~1;
            B9158_AT(u32, 0x104) = 0;
            B9158_AT(u8, 0x2A8) = 3;
            B9158_AT(u8, 0x2A9) = 1;
            B9158_AT(u8, 0x2AA) = 0;
            B9158_AT(u32, 0x2AC) = 0;
            B9158_AT(u8, 0x2B1) = 1;
            B9158_AT(u8, 0x5DA) = 6;
        }
        sub_0807E334(a);
        B9158_AT(u16, 0x2C2) = 0x20;
    } else {
        s32 w = *(s16 *)(*(u8 **)(gUnk_030052F4 + 4) + 4);
        s32 gx = Mod(cell, w);
        s32 gy = Div(cell, w);

        st[0] = (gx << 8) + 0x80;
        st[1] = (b9158_height(cell, (u8 *)st) & 0xF00) | 0;
        st[2] = (gy << 8) + 0x80;
        mesh = B9158_AT(u32, 0x5D0);
        if (!Nav_IsPositionInMesh((void *)mesh, a + 0x5C, st)) {
            B9158_AT(u16, 0x2D4) &= ~0x200;
            r = 0;
        } else {
            r = Terrain_ApplyBlocked((void *)mesh, a + 0x5C, st);
            if (r != 0) {
                if (r != 2)
                    Memory_Clear(a + 0x64, 8);
                r = 1;
            } else {
                B9158_AT(u16, 0x2D4) &= ~0x200;
            }
        }
        if (r == 0)
            Callback_InvokeActor(a, *(void **)(a + 0x5F8));
    }
}

#ifdef NONMATCHING
// fuzzy: 12.0%
// scope shape 1425 (differences once low registers are set aside)
// confidence=low: timed wander/turn state of a ground enemy: picks a heading, probes terrain heights and mesh, queues state switch
void sub_080B9158(u8 *a)
{
    s32 d;
    s32 speed;
    s32 v;

    if (B9158_AT(u8, 0x2B2)) {
        B9158_AT(u8, 0x2B2) = 0;
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
        speed = (gUnk_0203B400[gUnk_03005308] & 3) + 10;
        d = SceneActorTerrain_FindOpenNeighborDirection(a, B9158_AT(u8, 0x2B6));
        if (d < 0) {
            SceneActorAnimation_SelectDirectionalEntry(a, 0xF);
            B9158_AT(u16, 0x414) |= 4;
            B9158_AT(u16, 0x2D4) |= 0x80;
            B9158_AT(u16, 0x418) = B9158_AT(u16, 0x416);
            b9158_Enter(a);
            *(u32 *)(B9158_AT(u8 *, 0x3D0) + 0x6DC) |= 0x80;
            return;
        }
        v = gUnk_086149C4[((d << 6) + 0x40) & 0xFF] * speed;
        B9158_AT(s16, 0x2EC) = v >= 0 ? v >> 12 : -((-v) >> 12);
        B9158_AT(s16, 0x2EE) = 0;
        v = gUnk_086149C4[(d << 6) & 0xFF] * speed;
        B9158_AT(s16, 0x2F0) = v >= 0 ? v >> 12 : -((-v) >> 12);
        B9158_AT(u8, 0x59) = ((d + 2) & 3) << 6;
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
        B9158_AT(u16, 0x2EE) = (gUnk_0203B400[gUnk_03005308] & 0xF) + 12;
        SceneActorAnimation_SelectDirectionalEntry(a, 0xE);
    }

    if (B9158_AT(u32, 0x2D0) & 0x10) {
        SceneActorAnimation_SelectDirectionalEntry(a, 0xF);
        b9158_Enter(a);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B9158);
#endif
