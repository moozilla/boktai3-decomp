#include "global.h"
#include "scene/terrain/terrain_cell_probe.h"
#include "engine/ram.h"
#include "system/game_state/game_state.h"
#include "player/pile_trap/scene_marker.h"

void AnimData_SelectPaletteBank(void *, s32);
s32 Collider_Refresh(void *);
void PileTrap_DecrementDuration(void *);

s32 PileTrap_Update(struct SceneMarker *b)
{
    s32 st;
    s32 *pst, *q;
    if ((gSceneEffectPalettePhase & 3) == 0) {
        if (TerrainModifiers_FindCellOverride(cellIndexAt((struct Pos16 *)&b->collider.position),
                                              0))
            b->colliderState = 0;
    }
    pst = &b->colliderState;
    st = *pst;
    q = pst;
    if (st == 3) {
        u32 *cnt;
        AnimData_SelectPaletteBank(&b->secondaryData, 0x64);
        AnimData_SelectPaletteBank(&b->primaryData, 0x223);
        SceneMarkerSprite_SetFrame(&b->primary, 5);
        cnt = &b->state;
        if (*cnt > 5) {
            b->secondary.flags &= ~1;
            Collider_Refresh(&b->collider);
        }
        PileTrap_DecrementDuration(b);
        if (gGameState->unk77C <= 0)
            *q = 2;
        (*cnt)++;
    } else if (st == 1) {
        u32 c;
        u32 off = 0x14C;
        c = ++*(u32 *)((u8 *)b + off);
        if (c <= 29) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x222);
            SceneMarkerSprite_SetFrame(&b->primary, 0);
        } else if (c <= 59) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x222);
            SceneMarkerSprite_SetFrame(&b->primary, st);
        } else if (c <= 65) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x221);
            SceneMarkerSprite_SetFrame(&b->primary, st);
        } else if (c <= 71) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x220);
            SceneMarkerSprite_SetFrame(&b->primary, st);
        } else if (c <= 91) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x21F);
            SceneMarkerSprite_SetFrame(&b->primary, st);
        } else if (c <= 97) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x21F);
            SceneMarkerSprite_SetFrame(&b->primary, 2);
        } else if (c <= 103) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x21F);
            SceneMarkerSprite_SetFrame(&b->primary, 3);
        } else if (c <= 109) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x21F);
            SceneMarkerSprite_SetFrame(&b->primary, 4);
        } else if (c <= 129) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x21F);
            SceneMarkerSprite_SetFrame(&b->primary, 5);
        } else {
            *(s32 *)((u8 *)b + off) = 0;
            *q = 2;
        }
        b->secondary.flags |= 1;
        b->primary.flags &= ~1;
    } else if (st == 2) {
        u32 c;
        u32 *cnt = &b->state;
        c = ++*cnt;
        if (c <= 19) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x21F);
        } else if (c <= 39) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x220);
        } else if (c <= 59) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x221);
        } else if (c <= 79) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x222);
        } else if (c <= 99) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x221);
        } else if (c <= 119) {
            AnimData_SelectPaletteBank(&b->primaryData, 0x220);
        } else {
            *cnt = 0;
        }
        SceneMarkerSprite_SetFrame(&b->primary, 5);
        b->secondary.flags |= 1;
    } else {
        b->state = 0;
        b->secondary.flags |= 1;
    }
    return 0;
}
