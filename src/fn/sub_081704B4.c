#define SceneHud_UpdateActionSwap sub_081704B4
#define gSceneInputKeys           gUnk_03005260
#include "global.h"
#include "scene_hud_display.h"
extern u16 gUnk_03005260[];
int sub_0816B59C(void *, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void sub_08166B7C(void *, int);
u32 PauseMenu_CanSwapCategoryEntries(void *, u32, u32, u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
void sub_0816DA58(void *, int, int, int);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0817061C(void);
void PauseMenu_OnSwapSelectionUpdate(void);
void Hud_RestoreGridSelection(void *);
void Hud_SetGridPosition(void *, int, int);
void sub_081704B4(struct SceneHudState *p)
{
    u8 *input = p->actionInput;
    u8 *cursor, *selection, *category;
    struct SceneInputWidget *widget;
    if (sub_0816B59C(input, 240)) {
        sub_0816D920((u8 *)p + 0x1620, p->actionCursor);
        sub_0816D868(p);
    }
    sub_08166B7C(p, 4);
    if (gUnk_03005260[1] & 1) {
        cursor = &p->actionCursor;
        if (!PauseMenu_CanSwapCategoryEntries(p, *cursor, *(selection = &p->actionSelection), 4)) {
            Sound_PlaySong(0x192);
        } else {
            Sound_PlaySong(0x38A);
            category = &p->actionCategory;
            if (Inventory_GetQuantityByStorage(*category, *selection) >= 0)
                sub_0816DA58(p, 0, *selection, *cursor);
            if (Inventory_GetQuantityByStorage(*category, *cursor) >= 0)
                sub_0816DA58(p, 1, *cursor, *selection);
            p->categoryWidgets[*selection + 10].flags |= 1;
            p->categoryWidgets[*cursor + 10].flags |= 1;
            Hud_SetInteractionCallback(p, sub_0817061C, 1);
        }
    } else if (gUnk_03005260[1] & 2) {
        Hud_SetInteractionCallback(p, PauseMenu_OnSwapSelectionUpdate, 1);
        Hud_RestoreGridSelection(input);
        Hud_SetGridPosition(input, p->actionInput[1], p->actionInput[2]);
        sub_0816D920((u8 *)p + 0x1620, p->actionCursor);
        widget = &p->categoryWidgets[6];
        widget->flags |= 1;
        sub_0816D868(p);
        Sound_PlaySong(222);
    }
}
