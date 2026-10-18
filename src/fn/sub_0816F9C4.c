#define SceneHud_UpdateTimedMessage    sub_0816F9C4
#define SceneHud_UpdateActionSelection sub_0816FF4C
#define gSceneInputKeys                gUnk_03005260
#define gGameState                     gUnk_02000710
#include "global.h"
s32 MessageWindow_Clear(void);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_ShowMessage(s32);
#include "scene_hud_display.h"
extern u16 gUnk_03005260[];
s32 TextWindow_SetScript(s32, const u8 *);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
void TextWindow_StartPlayback(s32);
void TextWindow_Clear(s32);
void Inventory_GetEquipmentDescriptor(s32, void *);
void sub_08174494(s32, void *, void *);
void sub_0816D98C(void *);
void sub_0816FF4C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void Hud_SaveGridSelection(void *);
void sub_081672FC(void *);
void sub_08166B7C(void *, int);
void sub_0816701C(void *, int, int, int, int, int);
void sub_081663A0(void *, int, int, int, int);
void sub_0816F9C4(struct SceneHudState *p)
{
    u8 text[32];
    u8 *q;
    int z;
    if (p->actionTransferDelay == 0) {
        MessageWindow_Clear();
        MessageWindow_SetBounds(0, 18, 30, 2);
        Inventory_GetEquipmentDescriptor(
            *(s16 *)((u8 *)gUnk_02000710 + 0x68 + 2 * *(s16 *)((u8 *)gUnk_02000710 + 0x50)), text);
        TextWindow_SetBounds(p->timedMessageWindow, 0, 16, 30, 2);
        TextWindow_SetScript(p->timedMessageWindow, p->timedMessageScript);
        sub_08174494(p->timedMessageWindow, text, p->unknownA44);
        TextWindow_StartPlayback(p->timedMessageWindow);
    }
    if (p->actionTransferDelay == 8) {
        MessageWindow_SetMessageTable(p->actionTextScript);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(120);
    }
    p->actionTransferDelay++;
    z = 0;
    if (p->actionTransferDelay > 89 || (p->actionTransferDelay > 15 && (gUnk_03005260[1] & 1))) {
        TextWindow_Clear(p->timedMessageWindow);
        MessageWindow_SetBounds(0, 16, 30, 2);
        sub_0816D98C(p);
        Hud_SetInteractionCallback(p, sub_0816FF4C, 1);
        q = (u8 *)p + 0x4384;
        sub_0816CDB8(q, 1, 0);
        sub_0816D920((u8 *)p + 0x1620, ((u8 *)p)[0x4387]);
        sub_0816D868(p);
        Hud_SaveGridSelection(q);
        sub_081672FC(p);
        sub_08166B7C(p, 4);
        sub_0816701C(p, 0, 0, 0, 1, z);
        sub_081663A0(p, 1, 0, 0, z);
    }
}
