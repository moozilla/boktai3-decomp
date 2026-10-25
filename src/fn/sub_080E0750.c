#include "global.h"

/* grid cell id of tile (x, y), 0 when outside the map */

/* terrain height at a cell, minus the slope offset of the probe position */

/* is the move from pos to dest accepted by the nav mesh and terrain */

/* the shared actor-state stores of every branch */

/* pick a new facing different from the current one (up to four random draws) */

/* snap to the cell-center target and reject it against the mesh, in a local pos */

// confidence=low: actor knockback/slide landing update: slides toward the target, snaps to a free cell, rerolls facing
#define AT(T, off) (*(T *)((u8 *)arg0 + (off)))

void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void *TerrainModifiers_FindCellOverride(s32, s32);
void sub_0807E334(void *);
s32 Mod(s32, s32);
s32 Div(s32, s32);
s32 Nav_IsPositionInMesh(void *, void *, void *);
s32 Terrain_ApplyBlocked(void *, void *, void *);
void Memory_Clear(void *, u32);
void Callback_InvokeActor(void *, void *);

extern u8 *gUnk_030052F4;
extern u32 gUnk_030052F8;
extern u32 gUnk_030052FC;
extern u32 gUnk_030053F4;
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern u8 *gUnknown_02000580[];

/* grid cell id of tile (x, y), 0 when outside the map */
static inline s32 f0750_CellId(s32 x, s32 y)
{
    if (x < 0 || y < 0 || (u32)x >= gUnk_030052F8 || (u32)y >= gUnk_030052FC)
        return 0;
    return *(u16 *)(gUnk_030052F4 + 0x24 + y * 2) + x;
}

/* terrain height at a cell, minus the slope offset of the probe position */
static inline s32 f0750_CellHeight(s32 cell, u8 *pos)
{
    u8 *p;
    s32 b, h;

    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else
        p = *(u8 **)(gUnk_030052F4 + 4) + cell * 4 + 0xC;
    b = *p;
    h = (b & 0xF) << 8;
    if ((b >> 4) == 1)
        h -= pos[4];
    else if ((b >> 4) == 2)
        h -= pos[0];
    return h;
}

/* is the move from pos to dest accepted by the nav mesh and terrain */
static inline s32 f0750_NavMove(u8 *arg0, void *mesh, void *pos, void *dest)
{
    s32 r;

    if (!Nav_IsPositionInMesh(mesh, pos, dest)) {
        AT(u16, 0x2D4) &= ~0x200;
        return 0;
    }
    r = Terrain_ApplyBlocked(mesh, pos, dest);
    if (r) {
        if (r != 2)
            Memory_Clear(arg0 + 0x64, 8);
        return 1;
    }
    AT(u16, 0x2D4) &= ~0x200;
    return 0;
}

/* the shared actor-state stores of every branch */
static inline void f0750_SetState(u8 *arg0, u8 a5, void *cb, u32 c8, u8 a8, u8 a9, u8 aa)
{
    AT(u8, 0x2B2) = 1;
    AT(u8, 0x2B0) = 0;
    AT(u8, 0x2B5) = a5;
    AT(void *, 0x21C) = cb;
    AT(u16, 0x2C8) = c8;
    AT(u32, 0x114) &= ~1;
    AT(u32, 0x104) = c8;
    AT(u8, 0x2A8) = a8;
    AT(u8, 0x2A9) = a9;
    AT(u8, 0x2AA) = aa;
    AT(u32, 0x2AC) = 0;
    AT(u8, 0x2B1) = 1;
}

/* pick a new facing different from the current one (up to four random draws) */
static inline void f0750_RerollFacing(u8 *arg0)
{
    u8 *sb = arg0 + 0x3D4;
    s32 facing = arg0[0x59];
    s32 i;
    s32 rnd = facing;
    u8 *q;

    for (i = 0; i <= 3; i++) {
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
        rnd = (gUnk_0203B400[gUnk_03005308] & 3) << 6;
        if (rnd != facing)
            break;
        rnd = facing;
    }
    sb[0x204] = rnd;
    q = sb + 0x8C;
    *(u32 *)q = AT(u32, 0x21C);
    q[7] = AT(u8, 0x2B5);
    q[4] = AT(u8, 0x2A8);
    q[5] = AT(u8, 0x2A9);
    q[6] = AT(u8, 0x2AA);
    sb[0x205] = facing;
    f0750_SetState(arg0, 0x1B, AT(void *, 0x5F0), 0, 3, 1, 0);
    sb[0x206] = 6;
}

/* snap to the cell-center target and reject it against the mesh, in a local pos */
static inline s32 f0750_MeshCheck(u8 *arg0, s32 cell)
{
    u16 loc[3];
    s32 cx, cy;
    s32 h;
    u8 *p;

    cx = Mod(cell, *(s16 *)(*(u8 **)(gUnk_030052F4 + 4) + 4));
    cy = Div(cell, *(s16 *)(*(u8 **)(gUnk_030052F4 + 4) + 4));
    loc[0] = cx * 256 + 0x80;
    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else
        p = *(u8 **)(gUnk_030052F4 + 4) + cell * 4 + 0xC;
    loc[1] = (*p & 0xF) << 8;
    loc[2] = cy * 256 + 0x80;
    h = f0750_NavMove(arg0, *(void **)(arg0 + 0x5D0), arg0 + 0x5C, loc);
    return h;
}

#ifdef NONMATCHING
// fuzzy: 22.3%
// scope shape 1271 (differences once low registers are set aside)
// confidence=low: actor knockback/slide landing update: slides toward the target, snaps to a free cell, rerolls facing
void sub_080E0750(u8 *arg0)
{
    s32 dx, dy;
    s32 res, cell, idx, flag;
    u16 *p;
    u16 *pt;
    u8 *q;
    u8 *sb;
    u8 *dest;

    if (AT(u8, 0x2B2)) {
        AT(u8, 0x2B2) = 0;
        AT(s16, 0x2EC) = AT(s16, 0x2EC) / 4;
        AT(s16, 0x2F0) = AT(s16, 0x2F0) / 4;
        SceneActorAnimation_SelectDirectionalEntry(arg0, 3);
        AT(u16, 0x2D4) = (AT(u16, 0x2D4) | 0x20) & ~0x100;
    }
    AT(u32, 0x2D0) |= 0x10000;
    dx = (AT(s16, 0x310) - AT(s16, 0x318)) / 4;
    dy = (AT(s16, 0x314) - AT(s16, 0x31C)) / 4;
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dy > 200)
        dy = 200;
    else if (dy < -200)
        dy = -200;
    AT(u16, 0x64) += dx;
    AT(u16, 0x68) += dy;
    AT(u16, 0x318) += dx;
    AT(u16, 0x31C) += dy;
    if (!(dx == 0 && dy == 0))
        goto done_flag;
    if (AT(u8, 0x2B3) == 0)
        goto done_flag;

    sb = arg0 + 0x3D4;
    AT(u8, 0x2B0) = 1;
    AT(u16, 0x2D4) &= ~0x20;
    if (AT(u32, 0x2DC) & 0x80000) {
        AT(u16, 0x2D6) |= 2;
        return;
    }

    if (AT(u16, 0x2D4) & 0x80) {
        /* ---- mode A ---- */
        AT(u32, 0x2CC) |= 0x80;
        AT(u8 *, 0xDC) = gUnknown_02000580[0] + 0x24;
        p = AT(u16 *, 0x5D4);
        pt = *(u16 **)(arg0 + 0x448 + 8);
        p[0] = pt[0];
        p[2] = pt[1];
        p[1] = f0750_CellHeight(f0750_CellId((s16)pt[0] >> 8, (s16)pt[1] >> 8), (u8 *)p);
        if (gUnk_030053F4 & 0x4800) {
            q = AT(u8 *, 0xDC);
            if (q == 0)
                goto a_callback;
            idx = -1;
            if (q[8] <= 3)
                idx = q[8];
            if (idx < 0)
                goto a_callback;
            cell = (*(u16 **)(gUnknown_02000580[idx] + 0x40))[3];
        } else {
            q = AT(u8 *, 0xDC);
            cell = f0750_CellId((s16) * (u16 *)(q + 0xC) >> 8, (s16) * (u16 *)(q + 0x10) >> 8);
            idx = 0;
        }
        if (cell != AT(u16, 0x2F4) && !(*(u32 *)(gUnknown_02000580[idx] + 0x20) & 0x20000))
            goto a_mesh;
        if (AT(u16, 0x2D4) & 0x400) {
            res = 0;
            goto a_after;
        }
        *(u16 *)(sb + 0x40) &= ~4;
        flag = AT(u16, 0x2D4) & 0x80;
        if (flag) {
            f0750_SetState(arg0, 0x1A, *(void **)(sb + 0x48), 0, 0, 0, 0xC);
        } else {
            f0750_SetState(arg0, 1, AT(void *, 0x228), 0, 0, 0, 5);
            p = *(u16 **)(sb + 0x200);
            pt = *(u16 **)(sb + 0x74 + 8);
            p[0] = pt[0];
            p[2] = pt[1];
            p[1] = f0750_CellHeight(f0750_CellId((s16)pt[0] >> 8, (s16)pt[1] >> 8), (u8 *)p);
            p = *(u16 **)(sb + 0x200);
            p[1] = f0750_CellHeight(f0750_CellId((s16)p[0] >> 8, (s16)p[2] >> 8), (u8 *)p);
            p[3] = 0;
        }
        if (!(AT(u16, 0x2D4) & 0x400) && !(AT(u16, 0x414) & 4))
            f0750_RerollFacing(arg0);
        sub_0807E334(arg0);
        AT(u16, 0x2C2) = 0x20;
        res = 0;
        goto a_after;
    a_mesh:
        res = f0750_MeshCheck(arg0, cell);
        if (res)
            goto a_moved;
    a_callback:
        Callback_InvokeActor(arg0, *(void **)(sb + 0x224));
        res = 0;
        goto a_after;
    a_moved:
        res = 1;
    a_after:
        if (!res) {
            dest = AT(u8 *, 0xDC) + 0xC;
            res = f0750_NavMove(arg0, *(void **)(sb + 0x1FC), arg0 + 0x5C, dest);
            if (!res) {
                Callback_InvokeActor(arg0, *(void **)(sb + 0x224));
                return;
            }
        }
        *(u16 *)(sb + 0x40) |= 4;
        f0750_SetState(arg0, 0x1A, *(void **)(sb + 0x48), 0, 0, 0, 0xC);
        return;
    } else {
        /* ---- mode B ---- */
        AT(u8 *, 0xDC) = gUnknown_02000580[0] + 0x24;
        AT(u32, 0x2CC) |= 0x10;
        AT(u16 *, 0x5D4)[3] = 0;
        p = AT(u16 *, 0x5D4);
        pt = *(u16 **)(arg0 + 0x448 + 8);
        p[0] = pt[0];
        p[2] = pt[1];
        p[1] = f0750_CellHeight(f0750_CellId((s16)pt[0] >> 8, (s16)pt[1] >> 8), (u8 *)p);
        if (gUnk_030053F4 & 0x4800) {
            q = AT(u8 *, 0xDC);
            if (q == 0)
                goto b_callback;
            idx = -1;
            if (q[8] <= 3)
                idx = q[8];
            if (idx < 0)
                goto b_callback;
            cell = (*(u16 **)(gUnknown_02000580[idx] + 0x40))[3];
        } else {
            q = AT(u8 *, 0xDC);
            cell = f0750_CellId((s16) * (u16 *)(q + 0xC) >> 8, (s16) * (u16 *)(q + 0x10) >> 8);
            idx = 0;
        }
        if (cell != AT(u16, 0x2F4) && !(*(u32 *)(gUnknown_02000580[idx] + 0x20) & 0x20000))
            goto b_mesh;
        if (AT(u16, 0x2D4) & 0x400) {
            res = 0;
            goto b_after;
        }
        *(u16 *)(sb + 0x40) &= ~4;
        flag = AT(u16, 0x2D4) & 0x80;
        if (flag) {
            f0750_SetState(arg0, 0x1A, *(void **)(sb + 0x48), 0, 0, 0, 0xC);
        } else {
            f0750_SetState(arg0, 1, AT(void *, 0x228), 0, 0, 0, 5);
            p = *(u16 **)(sb + 0x200);
            pt = *(u16 **)(sb + 0x74 + 8);
            p[0] = pt[0];
            p[2] = pt[1];
            p[1] = f0750_CellHeight(f0750_CellId((s16)pt[0] >> 8, (s16)pt[1] >> 8), (u8 *)p);
            p = *(u16 **)(sb + 0x200);
            p[1] = f0750_CellHeight(f0750_CellId((s16)p[0] >> 8, (s16)p[2] >> 8), (u8 *)p);
            p[3] = 0;
        }
        if (!(AT(u16, 0x2D4) & 0x400) && !(AT(u16, 0x414) & 4))
            f0750_RerollFacing(arg0);
        sub_0807E334(arg0);
        AT(u16, 0x2C2) = 0x20;
        res = 0;
        goto b_after;
    b_mesh:
        res = f0750_MeshCheck(arg0, cell);
        if (res)
            goto b_moved;
    b_callback:
        Callback_InvokeActor(arg0, *(void **)(sb + 0x224));
        res = 0;
        goto b_after;
    b_moved:
        res = 1;
    b_after:
        if (!res) {
            dest = AT(u8 *, 0xDC) + 0xC;
            res = f0750_NavMove(arg0, *(void **)(sb + 0x1FC), arg0 + 0x5C, dest);
            if (!res) {
                Callback_InvokeActor(arg0, *(void **)(sb + 0x224));
                return;
            }
        }
        f0750_SetState(arg0, 1, AT(void *, 0x228), 0, 0, 0, 7);
        AT(u16, 0x2C2) = 0x10;
        return;
    }
done_flag:
    AT(u32, 0x2CC) |= 0x2000;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080E0750);
#endif
