#define SceneHud_UpdateActionChoice sub_08172CE8
#include "global.h"
#include "scene_hud_display.h"
extern u16 gUnk_03005260[];
void Sound_PlaySong(u32);
int sub_0816B59C(void *, int);
void sub_081727AC(void *, u32);
void sub_081723D8(void *);
void sub_08166B7C(void *, int);
void PauseInventory_ReturnToEquipmentSelection(void *);
void Hud_SaveGridSelection(void *);
void sub_08172850(void *, u32, u32);
void sub_08172E70(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08172CE8(struct SceneHudState *p)
{
    u8 *q = p->actionInput;
    u8 *cursor;
    u32 mode;
    u8 value;
    if (sub_0816B59C(q, 0xf0)) {
        sub_081727AC((u8 *)p + 0x1620, p->actionCursor);
        sub_081723D8(p);
    }
    cursor = &p->actionCursor;
    value = *cursor;
    mode = 1;
    if (value <= 7)
        mode = 0;
    sub_08166B7C(p, mode);
    if (gUnk_03005260[1] & 2) {
        PauseInventory_ReturnToEquipmentSelection(p);
        Sound_PlaySong(0xde);
    } else if (1 & gUnk_03005260[1]) {
        Sound_PlaySong(0xdd);
        Hud_SaveGridSelection(q);
        p->categoryWidgets[6].flags &= ~1;
        sub_081727AC((u8 *)p + 0x1680, p->actionSelection);
        if (*cursor <= 7)
            sub_08172850(q, 0, 1);
        else
            sub_08172850(q, 0, 2);
        Hud_SetInteractionCallback(p, sub_08172E70, 1);
    }
}
