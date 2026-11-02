// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_hud_display.h"
#include "ram.h"
s32 Mod(s32, s32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void Sound_PlaySong(u32);
void SceneHud_GetActionIconPosition(struct SceneVec *, s32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 Selection_GetDisplayFrame(s32);
void SceneHud_BeginIconMotion(u8 *, u32, s32, u32, void *, void *, u32, u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void PauseMenu_OnCharacterSelectionUpdate(void);
#define NextRandom() (*index = (*index + 1) & 0x3ff, table[*index])

void sub_0816E570(void *owner)
{
    struct SceneVec pos;
    struct SceneInputPackedPosition velocity;
    struct SceneHudState *ui = owner;
    u8 *kind;
    u16 speed;
    register const u16 *table asm("r6");
    u32 *index;

    kind = &ui->actionSelection;
    ui->categoryWidgets[*kind + 10].flags |= 1;
    SceneHud_GetActionIconPosition(&pos, *kind);
    pos.x += 8;
    pos.y += 8;
    table = gRandomTable;
    index = &gRandomIndex;
    velocity.x = -6 - Mod(NextRandom(), 3);
    velocity.y = -5 - Mod(NextRandom(), 5);
    speed = Mod(NextRandom(), 10) + 5;
    if (NextRandom() & 1)
        speed = -speed;
    SceneHud_BeginIconMotion(
        owner, 0,
        Selection_GetDisplayFrame(Inventory_GetQuantityByStorage(ui->actionCategory, *kind)),
        ui->categoryWidgets[*kind + 10].paletteBank, &pos, &velocity, 0, 0);
    {
        s16 *spin = (s16 *)&ui->widgetMotions[0].timer;
        *spin = speed;
    }
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    Sound_PlaySong(0x27d);
    Hud_SetInteractionCallback(owner, PauseMenu_OnCharacterSelectionUpdate, 1);
}
