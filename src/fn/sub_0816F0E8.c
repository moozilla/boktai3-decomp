#define MenuTransition_SetCallback Hud_SetInteractionCallback
#include "global.h"
#include "input.h"
void SceneHud_RefreshActionIcons(void *);
void SceneHud_UpdateActionSelection(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void Hud_SaveGridSelection(void *);
void InventoryPanel_ShowStatusIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
struct MenuMessageView {
    u8 pad[0x3c];
    void *resource;
    u8 unknown40[0xa4e - 0x40];
    u16 timer;
};
struct MenuFlickerTarget {
    u8 unknown0[0x424];
    u16 x, unknown426, y, z;
};
struct MenuFlickerView {
    u8 unknown0[0xa40];
    struct MenuFlickerTarget *target;
    u8 unknowna44[10];
    u16 timer;
};
s32 SceneHud_TickPaletteBlend(void *);
void sub_0816D634(void *, u32);
void Hud_DrawDefaultHealthMeter(void *, u32, u32, u32);
void Hud_DrawDefaultEnergyMeter(void *, s32, s32, s32, s32, s32);
void MenuTransition_UpdateWidgetFlicker(u8 *p)
{
    u32 r;
    u8 *q, *a;
    if (((struct MenuFlickerView *)p)->timer > 16)
        SceneHud_TickPaletteBlend(p);
    if ((((struct MenuFlickerView *)p)->timer & 7) <
        7 - (((struct MenuFlickerView *)p)->timer >> 3)) {
        ((struct MenuFlickerView *)p)->target->x = *(u16 *)(p + 0x4868);
        ((struct MenuFlickerView *)p)->target->y = *(u16 *)(p + 0x486c);
    } else {
        ((struct MenuFlickerView *)p)->target->x = *(u16 *)(p + 0x486a);
        ((struct MenuFlickerView *)p)->target->y = *(u16 *)(p + 0x486e);
    }
    Hud_DrawDefaultHealthMeter(p, 1, 5, 3);
    Hud_DrawDefaultEnergyMeter(p, 1, 5, 4, ((struct MenuFlickerView *)p)->target->y,
                               ((struct MenuFlickerView *)p)->target->z);
    {
        u32 next = ((struct MenuFlickerView *)p)->timer + 1;
        r = 0;
        ((struct MenuFlickerView *)p)->timer = next;
    }
    if (((struct MenuFlickerView *)p)->timer > 55) {
        sub_0816D634(p, p[0x438a]);
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionSelection, 1);
        q = p + 0x4384;
        sub_0816CDB8(q, 1, 0);
        a = p + 0x1620;
        SceneHud_GetActionIconPosition(a, p[0x4387]);
        SceneHud_ShowActionPreview(p);
        Hud_SaveGridSelection(q);
        InventoryPanel_ShowStatusIcons(p);
        MenuDisplay_UpdateActionPrompt(p, 4);
        InventoryPanel_DrawStatComparison(p, 0, 0, 0, 1, r);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, r);
    }
}
