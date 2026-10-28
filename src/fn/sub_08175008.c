#include "global.h"
#include "input.h"
// Remaining: input/array address allocation and constant scheduling.
struct AllocHud {
    u8 pad[0x48a8];
    u16 remaining;
    u8 index, description;
    u8 values[3];
    u8 pad2;
    s16 deltas[3], offsets[3];
};
struct AllocOwner {
    u8 pad[0x41c];
    u16 stats[3];
};
struct AllocSprite {
    u32 pad[2];
    u32 flags;
};
void ResultPanel_DrawCappedCounters(void *, s32, s32, s32);
void ResultPanel_DrawCappedSmallCounters(void *, s32, s32, s32);
void PauseMenu_HidePromptIcons(void *);
u32 MenuInput_ReadDirectionRepeat(void *);
void Presentation_UpdateListCursor(void *);
void Sound_PlaySong(u32);
void Presentation_DrawEntryCount(void *);
void InventoryGrid_LayoutStatMarkers(void *);
void PauseMenu_SetConfirmationIndicator(void *, u32);
void MenuDisplay_SetPaletteOverrides(void *, u32, u32, u32, u32);
void MenuSelection_ProcessConfirmation(void);
void Hud_SetInteractionCallback(void *, void (*)(void), u32);
void Presentation_InitListValues(void *);
void PauseMenu_SetPromptIconPositions(void *, u32, u32);
void SceneHud_SetPairedIcons(void *, u32, s32);
void Anim_Step(void *, void *);
void InventoryGrid_UpdateStatPreview(void *);
void SceneCatalog_SetDescription(void *, s32);
static inline void setDescription(u8 *p, u32 n)
{
    p[0x48ab] = n;
}
void Presentation_UpdateAllocationSelection(u8 *p)
{
    s32 any;
    s32 i;
    s16 *counts;
    u32 key;
    p[0xa58] = 0;
    any = 0;
    counts = (s16 *)(p + 0x48b6);
    for (i = 2; i >= 0; i--) {
        if (*counts > 0) {
            any = 1;
            p[0xa58] = any;
        }
        counts++;
    }
    if (!any && (gPadState[0] & 4)) {
        u8 *modifier = p + 0xa58;
        any = 1;
        *modifier = any;
        ResultPanel_DrawCappedCounters(p, 1, 7, 8);
        ResultPanel_DrawCappedSmallCounters(p, 1, 7, 10);
        *(u32 *)(p + 0x3348) |= any;
        *(u32 *)(p + 0x33a8) |= any;
        *(u32 *)(p + 0x1608) |= any;
        *(u32 *)(p + 0x2aa8) |= any;
        PauseMenu_HidePromptIcons(p);
        goto finish;
    }
    key = MenuInput_ReadDirectionRepeat(p);
    if (key & 0x40) {
        if (!p[0x48aa]) {
            p[0x48aa] = 2;
        } else {
            p[0x48aa]--;
        }
        Presentation_UpdateListCursor(p);
        Sound_PlaySong(0xdc);
        setDescription(p, p[0x48aa] + 20);
    } else if (key & 0x80) {
        if (p[0x48aa] > 1) {
            p[0x48aa] = 0;
        } else {
            p[0x48aa]++;
        }
        Presentation_UpdateListCursor(p);
        Sound_PlaySong(0xdc);
        setDescription(p, p[0x48aa] + 20);
    } else if (key & 0x20) {
        u32 ix = p[0x48aa] << 1;
        u8 *b;
        s16 *n;
        b = p + 0x48b6;
        n = (s16 *)(b + ix);
        if (*n > 0) {
            (*n)--;
            (*(u16 *)(p + 0x48a8))++;
            goto changed;
        }
    } else if (({
                   any = 0x10;
                   any &= key;
                   any;
               })) {
        u16 *remaining = (u16 *)(p + 0x48a8);
        s32 index;
        s32 stat;
        s16 *n;
        u8 *owner;
        if (!(*remaining)) {
            goto display;
        }
        owner = *(u8 **)(p + 0xa40);
        index = p[0x48aa];
        {
            u32 idx2 = index << 1;
            u8 *b;
            u8 *ow = owner + 0x41c;
            stat = *(u16 *)(ow + idx2);
            b = p + 0x48b6;
            n = (s16 *)(b + idx2);
        }
        if ((stat + (*n)) > 99) {
            goto display;
        }
        if ((((struct AllocHud *)p)->values[index] + (*n)) > 99) {
            goto display;
        }
        (*n)++;
        (*remaining)--;
    changed:
        Presentation_DrawEntryCount(p);
        InventoryGrid_LayoutStatMarkers(p);
        Sound_PlaySong(0x19a);
    } else {
        if ((gPadState[1] & 1) && p[0xa58]) {
            Sound_PlaySong(0xdd);
            p[0x48ab] = 24;
            PauseMenu_SetConfirmationIndicator(p, 0);
            MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, any);
            Hud_SetInteractionCallback(p, MenuSelection_ProcessConfirmation, 1);
        } else if (gPadState[1] & 2) {
            Sound_PlaySong(0xde);
            Presentation_InitListValues(p);
            Presentation_DrawEntryCount(p);
            InventoryGrid_LayoutStatMarkers(p);
        }
    }
    do {
    display:
        *(u32 *)(p + 0x3348) &= ~1;
        if (p[0xa58]) {
            ((struct AllocSprite *)(p + 0x33a0))->flags &= ~1;
            *(u16 *)(p + 0x33c0) = 0;
            *(u16 *)(p + 0x33c2) = 8;
            PauseMenu_HidePromptIcons(p);
            PauseMenu_SetPromptIconPositions(p, 0, 8);
            SceneHud_SetPairedIcons(p, 1, -1);
        } else {
            *(u32 *)(p + 0x33a8) |= 1;
            PauseMenu_SetPromptIconPositions(p, 0, 8);
            SceneHud_SetPairedIcons(p, 1, -1);
        }
        *(u32 *)(p + 0x1608) &= ~1;
        Anim_Step(p + 0x1600, p + 0x64);
    finish:
        InventoryGrid_UpdateStatPreview(p);
        if (*(s8 *)(p + 0x48ab) >= 0) {
            SceneCatalog_SetDescription(p, *(s8 *)(p + 0x48ab));
            p[0x48ab] = 255;
        }
    } while (0);
}
