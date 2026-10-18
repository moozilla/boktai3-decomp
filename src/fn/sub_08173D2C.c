#include "global.h"
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowEquipmentDetails(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void SceneHud_BeginEquipmentSelection(void *);
void Sound_PlaySong(int);
void Hud_SaveGridSelection(void *);
void sub_08173E68(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
extern u16 gUnk_03005260[];
void sub_08173D2C(u8 *p)
{
    u8 *q = p + 0x4384;
    if (SceneInput_NavigateGridKeys(q, 0xf0)) {
        u8 *a = p + 0x1620;
        SceneHud_GetActionIconPosition(a, p[0x4387]);
        SceneHud_ShowEquipmentDetails(p);
        MenuDisplay_UpdateActionPrompt(p, 2);
    }
    if (gUnk_03005260[1] & 2) {
        SceneHud_BeginEquipmentSelection(p);
        Sound_PlaySong(0xde);
    } else if (1 & gUnk_03005260[1]) {
        u8 *a;
        Sound_PlaySong(0xdd);
        Hud_SaveGridSelection(q);
        *((u32 *)(p + 0x1668)) &= ~1;
        a = p + 0x1680;
        SceneHud_GetActionIconPosition(a, p[0x438a]);
        Hud_SetInteractionCallback(p, sub_08173E68, 1);
    }
}
