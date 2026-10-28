#include "global.h"
#include "engine_globals.h"

u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, s32);

// confidence=low: helper; resolves a cell's terrain byte, through the cell override table when one applies.

// confidence=low: helper; the grid row stride stored in the terrain header.

// confidence=low: helper; signed high byte of a u16 field, used as a grid coordinate.

// confidence=low: helper; direction index from the facing byte at +0x59.

// confidence=low: helper; applies the direction-dependent cursor adjustment to the 0x634 cell record.

// confidence=low: per-frame terrain cursor update for the scene actor in walk state 5; picks a cell from the facing direction and spawns the disc.

// confidence=low: helper; resolves a cell's terrain byte, through the cell override table when one applies.

// confidence=low: helper; the grid row stride stored in the terrain header.

// confidence=low: helper; signed high byte of a u16 field, used as a grid coordinate.

// confidence=low: helper; direction index from the facing byte at +0x59.

// confidence=low: helper; applies the direction-dependent cursor adjustment to the 0x634 cell record.

// confidence=low: per-frame terrain cursor update for the scene actor in walk state 5; picks a cell from the facing direction and spawns the disc.

// confidence=low: helper; resolves a cell's terrain byte, through the cell override table when one applies.

// confidence=low: helper; the grid row stride stored in the terrain header.

// confidence=low: helper; signed high byte of a u16 field, used as a grid coordinate.

// confidence=low: helper; direction index from the facing byte at +0x59.

// confidence=low: helper; applies the direction-dependent cursor adjustment to the 0x634 cell record.

// confidence=low: per-frame terrain cursor update for the scene actor in walk state 5; picks a cell from the facing direction and spawns the disc.

// confidence=low: helper; resolves a cell's terrain byte, through the cell override table when one applies.

// confidence=low: helper; the grid row stride stored in the terrain header.

// confidence=low: helper; signed high byte of a u16 field, used as a grid coordinate.

// confidence=low: helper; direction index from the facing byte at +0x59.

// confidence=low: helper; applies the direction-dependent cursor adjustment to the 0x634 cell record.

// confidence=low: per-frame terrain cursor update for the scene actor in walk state 5; picks a cell from the facing direction and spawns the disc.

// confidence=low: helper; resolves a cell's terrain byte, through the cell override table when one applies.

// confidence=low: helper; the grid row stride stored in the terrain header.

// confidence=low: helper; signed high byte of a u16 field, used as a grid coordinate.

// confidence=low: helper; direction index from the facing byte at +0x59.

// confidence=low: helper; applies the direction-dependent cursor adjustment to the 0x634 cell record.

// confidence=low: per-frame terrain cursor update for the scene actor in walk state 5; picks a cell from the facing direction and spawns the disc.

// confidence=low: helper; resolves a cell's terrain byte, through the cell override table when one applies.

// confidence=low: helper; the grid row stride stored in the terrain header.

// confidence=low: helper; signed high byte of a u16 field, used as a grid coordinate.

// confidence=low: helper; direction index from the facing byte at +0x59.

// confidence=low: helper; applies the direction-dependent cursor adjustment to the 0x634 cell record.

// confidence=low: per-frame terrain cursor update for the scene actor in walk state 5; picks a cell from the facing direction and spawns the disc.
extern u8 *gUnknown_02000580[];
void *TerrainModifiers_FindCellOverride(s32, s32);
u8 sub_0812E118(s32, s32, s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SunDiscPool_SpawnDisc(void *, void *, s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);

// confidence=low: helper; resolves a cell's terrain byte, through the cell override table when one applies.
static inline u8 *f08085DD0_cellPtr(s32 cell)
{
    u8 *over = (u8 *)TerrainModifiers_FindCellOverride(cell, 1);
    if (over != 0)
        return over + 4;
    return *(u8 **)(((u8 *)gTerrainMap) + 4) + cell * 4 + 0xC;
}

// confidence=low: helper; the grid row stride stored in the terrain header.
static inline s32 f08085DD0_stride(void)
{
    return *(s16 *)(*(u8 **)(((u8 *)gTerrainMap) + 4) + 4);
}

// confidence=low: helper; signed high byte of a u16 field, used as a grid coordinate.
static inline s32 f08085DD0_hiByte(u16 v)
{
    return (s8)(v >> 8);
}

// confidence=low: helper; direction index from the facing byte at +0x59.
static inline s32 f08085DD0_dir(u8 *base)
{
    s32 t = base[0x59] + 0x20;
    return (t >> 6) & 3;
}

// confidence=low: helper; applies the direction-dependent cursor adjustment to the 0x634 cell record.
static inline void f08085DD0_adjust(u8 *ctx, u8 *base, s32 dir)
{
    if (dir == 0) {
        *(u16 *)(ctx + 0x634) = *(u16 *)(ctx + 0x634) + 0x7F;
        *(u16 *)(ctx + 0x638) = *(u16 *)(base + 0x60);
    } else if (dir == 1) {
        *(u16 *)(ctx + 0x634) = *(u16 *)(base + 0x5C);
        *(u16 *)(ctx + 0x638) = *(u16 *)(ctx + 0x638) + 0x7F;
    } else if (dir == 2) {
        *(u16 *)(ctx + 0x634) = *(u16 *)(ctx + 0x634) - 0x7F;
        *(u16 *)(ctx + 0x638) = *(u16 *)(base + 0x60);
    } else if (dir == 3) {
        *(u16 *)(ctx + 0x634) = *(u16 *)(base + 0x5C);
        *(u16 *)(ctx + 0x638) = *(u16 *)(ctx + 0x638) - 0x7F;
    }
}

#ifdef NONMATCHING
// fuzzy: 35.4%
// scope shape 461 (differences once low registers are set aside)
// confidence=low: per-frame terrain cursor update for the scene actor in walk state 5; picks a cell from the facing direction and spawns the disc.
void sub_08085DD0(u8 *base)
{
    u8 *ctx = *(u8 **)(base + 0x3D0);
    u8 *walk = *(u8 **)(base + 4) + 0x48;
    s32 flagged;

    flagged = base[0x2B2];
    base[0x2B2] = 0;
    if (flagged) {
        u8 *b2;
        base[0x59] = ((base[0xE9] + 0x20) & 0xFF) >> 6 << 6;
        SceneActorAnimation_SelectDirectionalEntry(base, 5);
        if (*(u16 *)(ctx + 0x628) & 4) {
            *(u16 *)(walk + 8) = 2;
            *(u16 *)(walk + 0xE) = 0;
            *(u16 *)(ctx + 0x628) &= ~4;
        }
        b2 = *(u8 **)(base + 0xDC);
        if (gEngineFlags & 0x4800) {
            if (b2 != 0) {
                s32 idx = -1;
                if (b2[8] <= 3)
                    idx = b2[8];
                if (idx >= 0) {
                    u8 *e = gUnknown_02000580[idx];
                    *(u32 *)(ctx + 0x62C) = *(u32 *)(e + 0x30);
                    *(u32 *)(ctx + 0x630) = *(u32 *)(e + 0x34);
                }
            }
        } else {
            *(u32 *)(ctx + 0x62C) = *(u32 *)(b2 + 0xC);
            *(u32 *)(ctx + 0x630) = *(u32 *)(b2 + 0x10);
        }
    }

    if (*(u16 *)(walk + 8) == 5 && *(u16 *)(walk + 0xE) == 0) {
        s32 diff;
        s32 mag;
        u16 *p24;
        u16 *p28;
        u32 *p30;
        u16 *p2C;

        diff = base[0xE9] - ((((base[0x59] + 0x20) & 0xFF) >> 6) << 6);
        if (diff > 0x80)
            diff -= 0x100;
        else if (diff < -0x80)
            diff += 0x100;
        mag = diff >= 0 ? diff : -diff;

        if (mag <= 0x1F) {
            u8 *b2 = *(u8 **)(base + 0xDC);
            if (gEngineFlags & 0x4800) {
                if (b2 != 0) {
                    s32 idx = -1;
                    if (b2[8] <= 3)
                        idx = b2[8];
                    if (idx >= 0) {
                        u8 *e = gUnknown_02000580[idx];
                        *(u32 *)(ctx + 0x634) = *(u32 *)(e + 0x30);
                        *(u32 *)(ctx + 0x638) = *(u32 *)(e + 0x34);
                    }
                }
            } else {
                *(u32 *)(ctx + 0x634) = *(u32 *)(b2 + 0xC);
                *(u32 *)(ctx + 0x638) = *(u32 *)(b2 + 0x10);
            }
        } else {
            s32 cell = *(u16 *)(base + 0x2F4);
            u8 *cp;
            s32 kind;
            s32 v;
            s32 k2;
            s32 r4;
            s32 r5;
            s32 step;

            cp = f08085DD0_cellPtr(cell);
            kind = *cp >> 4;
            v = (*cp & 0xF) << 8;
            if (kind == 1)
                v -= base[0x60];
            else if (kind == 2)
                v -= base[0x5C];

            cp = f08085DD0_cellPtr(cell);
            k2 = *cp >> 4;

            p24 = (u16 *)(base + 0xB6);
            p28 = (u16 *)(base + 0xBE);
            p30 = (u32 *)(base + 0xD0);
            p2C = (u16 *)(base + 0xC6);
            switch ((((s32)base[0x59] + 0x20) >> 6) & 3) {
                case 0:
                    r4 = cell + 1;
                    r5 = 0;
                    do {
                        if ((u8)sub_0812E118(cell, r4, v, k2) == 0)
                            break;
                        r4++;
                        r5++;
                    } while (r5 < 1);
                    r5 = r4 - 1;
                    break;
                case 1:
                    step = f08085DD0_stride();
                    r4 = cell + step;
                    r5 = 0;
                    do {
                        if ((u8)sub_0812E118(cell, r4, v, k2) == 0)
                            break;
                        r4 += step;
                        r5++;
                    } while (r5 < 1);
                    r5 = r4 - step;
                    break;
                case 2:
                    r4 = cell - 1;
                    r5 = 0;
                    do {
                        if ((u8)sub_0812E118(cell, r4, v, k2) == 0)
                            break;
                        r4--;
                        r5++;
                    } while (r5 < 1);
                    r5 = r4 + 1;
                    break;
                case 3:
                    step = f08085DD0_stride();
                    r4 = cell - step;
                    r5 = 0;
                    do {
                        if ((u8)sub_0812E118(cell, r4, v, k2) == 0)
                            break;
                        r4 -= step;
                        r5++;
                    } while (r5 < 1);
                    r5 = r4 + step;
                    break;
                default:
                    r5 = cell;
                    break;
            }

            {
                s32 cx;
                s32 cy;
                cx = Mod(r5, f08085DD0_stride());
                cy = Div(r5, f08085DD0_stride());
                *(u16 *)(ctx + 0x634) = (cx << 8) + 0x80;
                cp = f08085DD0_cellPtr(r5);
                *(u16 *)(ctx + 0x636) = (*cp & 0xF) << 8;
                *(u16 *)(ctx + 0x638) = (cy << 8) + 0x80;
            }
            f08085DD0_adjust(ctx, base, f08085DD0_dir(base));
            {
                s32 x = f08085DD0_hiByte(*(u16 *)(ctx + 0x634));
                s32 y = f08085DD0_hiByte(*(u16 *)(ctx + 0x638));
                s32 gx = 0;
                if (x >= 0 && y >= 0 && x < gTerrainWidth && y < gTerrainHeight)
                    gx = *(u16 *)(((u8 *)gTerrainMap) + 0x24 + y * 2) + x;
                cp = f08085DD0_cellPtr(gx);
                kind = *cp >> 4;
                v = (*cp & 0xF) << 8;
                if (kind == 1)
                    v -= *(u8 *)(ctx + 0x638);
                else if (kind == 2)
                    v -= *(u8 *)(ctx + 0x634);
                *(u16 *)(ctx + 0x636) = v;
            }
        }

        {
            u32 vec = *(u32 *)(base + 0x5C);
            vec = (vec & 0xFFFF) | ((((s32)vec >> 16) + 0xDC) << 16);
            SunDiscPool_SpawnDisc(&vec, ctx + 0x634, *p24, *p28, *p30, *p2C, ctx[0x640], 30, -20);
        }
        Sound_PlaySong(0xD6);
    }

    if (base[0x2B3] != 0)
        base[0x2B0] = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08085DD0);
#endif
