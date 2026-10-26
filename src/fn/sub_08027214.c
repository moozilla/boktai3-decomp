#include "global.h"
#include "engine.h"

void SceneActors_RunFirstEventScript(void *, void *);
void SceneActors_RunSecondEventScript(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
u32 SceneActorGraphics_GetResourceId(void *);
void SceneActors_StopBackgroundFlash(void *);
void RouteSpritePool_RequestFinishAll(void);
void WirelessGame_NotifyOpponentDefeated(void);
void sub_08022790(void *, s32);
void sub_08022878(void *);
void GameState_SetEventFlag(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 sub_08027214_CellIndex(s32 x, s32 y)
{
    if (x < 0 || y < 0 || (u32)x >= gTerrainWidth || (u32)y >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[y] + x;
}

static inline s32 sub_08027214_Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

#ifdef NONMATCHING
// fuzzy: 51.0%
// scope shape 145 (differences once low registers are set aside)
// confidence=low: per-frame actor update; runs the state-switch on the byte at 0xA2 (with
// the arrival flag at 0xB1 and the 0x385 countdown) and bumps the 0xA4 phase counter.
void sub_08027214(u8 *p, s32 tick)
{
    u8 *pos;
    u8 *data;
    s32 x;
    s32 y;
    s32 cell;
    s32 d;
    s32 kind;

    if (p[0xB1]) {
        p[0xB1] = 0;
        SceneActors_RunFirstEventScript(p, p + 0x260);
        if (*(u32 *)(p + 0x358) == 1) {
            p[0x51] = p[0x342] + 0x80;
            p[0xA8] = ((p[0x51] + 0x60) >> 6) & 3;
            p[0xA9] = ((p[0x51] + 0x70) >> 5) & 7;
            SceneActors_StartDirectionalAnimation(p, p + 0x240, p + 0x220,
                                                  (*(u32 *)(p + 0x3AC) & 1) ? 0xB : 0x31, 2, 1, 4);
            p[0xA2] = 7;
            *(u32 *)(p + 0xA4) = 0;
            Sound_PlaySongChecked(0x359);
        } else {
            u32 flash = *(u32 *)(p + 0x3AC) & 1;
            SceneActors_StartUnmirroredAnimation(p, p + 0x240, p + 0x220, flash ? 0x20 : 0x46, 1,
                                                 0);
            *(u16 *)(p + 0x346) = 0;
            *(u16 *)(p + 0x348) = 0;
            p[0x343] = 0;
            if (*(u32 *)(p + 0x3AC) & 1) {
                p[0xA2] = 1;
                *(u32 *)(p + 0xA4) = 0;
                Sound_PlaySongChecked(0x1C5);
            } else {
                p[0xA2] = 0;
                *(u32 *)(p + 0xA4) = 0;
                Sound_PlaySongChecked(0x359);
            }
        }
        *(u32 *)(p + 0x390) = SceneActorGraphics_GetResourceId(p);
        p[0x38E] = 2;
        p[0x38F] = 1;
        if (p[0x38E] == 2) {
            p[0x38C] = 2;
        } else if (p[0x38E] == 1) {
            p[0x38C] = 0;
        }
        p[0x385] = 0;
        RouteSpritePool_RequestFinishAll();
        SceneActors_StopBackgroundFlash(p);
        if (gEngineFlags & 0x1000) {
            WirelessGame_NotifyOpponentDefeated();
        }
    }

    pos = p + 0x54;
    x = (s8)(*(u16 *)pos >> 8);
    y = (s8)(*(u16 *)(pos + 4) >> 8);
    cell = sub_08027214_CellIndex(x, y);
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data) {
        data += 4;
    } else {
        data = gTerrainMap->cells + (cell * 4 + 12);
    }
    kind = *data >> 4;
    d = (*data & 15) << 8;
    if (kind == 1) {
        d -= pos[4];
    } else if (kind == 2) {
        d -= pos[0];
    }
    if ((u32)d < (u32) * (s16 *)(p + 0x352)) {
        p[0x385] = 8;
    } else {
        cell = sub_08027214_CellIndex(x, y);
        data = TerrainModifiers_FindCellOverride(cell, 1);
        if (data) {
            data += 4;
        } else {
            data = gTerrainMap->cells + (cell * 4 + 12);
        }
        if (*(u16 *)(data + 2) & 1) {
            p[0x385] = 8;
        }
    }

    if (p[0x385]) {
        p[0x385]--;
        {
            s32 dx = *(s16 *)(p + 0x32C) - *(s16 *)(pos + 0);
            s32 dy = *(s16 *)(p + 0x330) - *(s16 *)(pos + 4);
            s32 angle = Math_Atan2(dx, dy);
            s16 *vec = (s16 *)(p + 0x5C);
            vec[0] = sub_08027214_Scale(gUnk_086149C4[(angle + 0x40) & 0xFF] * 20);
            vec[1] = 0;
            vec[2] = sub_08027214_Scale(gUnk_086149C4[angle & 0xFF] * 20);
        }
    }

    switch (p[0xA2]) {
        case 0:
            p[0x39B] = 8;
            if (p[0xA3] > 4) {
                p[0xA2] = 2;
                *(u32 *)(p + 0xA4) = 0;
                SceneActors_StartUnmirroredAnimation(p, p + 0x240, p + 0x220, 0x21, 2, 0);
                Sound_PlaySongChecked(0x350);
            }
            break;
        case 1:
            if ((tick & 3) == 0) {
                sub_08022790(p, 1);
            }
            p[0x39B] = 8;
            if (p[0xA3] > 4) {
                p[0xA2] = 2;
                *(u32 *)(p + 0xA4) = 0;
                SceneActors_StartUnmirroredAnimation(p, p + 0x240, p + 0x220, 0x21, 2, 0);
                *(u32 *)(p + 0x3AC) &= ~1;
                Sound_PlaySongChecked(0x359);
            }
            break;
        case 2:
            if (*(u16 *)(p + 0x254) == 1 && *(u16 *)(p + 0x24C) == 0) {
                Sound_PlaySongChecked(0x350);
            }
            if (p[0x25D] & 2) {
                p[0xA2] = 3;
                *(u32 *)(p + 0xA4) = 0;
                SceneActors_StartUnmirroredAnimation(p, p + 0x240, p + 0x220, 0x22, 2, 0);
                sub_08022878(p + 0x260);
            }
            break;
        case 3:
            if (p[0x25D] & 2) {
                p[0xA2] = 4;
                *(u32 *)(p + 0xA4) = 0;
                SceneActors_StartUnmirroredAnimation(p, p + 0x240, p + 0x220, 0x23, 2, 0);
            }
            break;
        case 4:
            if (*(u16 *)(p + 0x254) == 0xA && *(u16 *)(p + 0x24C) == 0) {
                Sound_PlaySongChecked(0x354);
            }
            if (p[0x25D] & 2) {
                p[0xA2] = 5;
                *(u32 *)(p + 0xA4) = 0;
                SceneActors_StartUnmirroredAnimation(p, p + 0x240, p + 0x220, 0x24, 2, 0);
            }
            break;
        case 5:
            if (p[0x25D] & 2) {
                p[0xA2] = 6;
                *(u32 *)(p + 0xA4) = 0;
                GameState_SetEventFlag(0x4D);
                SceneActors_RunSecondEventScript(p, p + 0x260);
            }
            break;
        case 6:
            break;
        case 7:
            p[0x39B] = 8;
            if (*(u32 *)(p + 0xA4) > 0xEF) {
                p[0xA2] = 5;
                *(u32 *)(p + 0xA4) = 0;
            }
            break;
        default:
            break;
    }
    *(u32 *)(p + 0xA4) += 1;
    *(u32 *)(p + 0x9C) |= 0xD;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08027214);
#endif
