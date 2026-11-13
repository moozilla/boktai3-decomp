#ifndef GUARD_WIRELESS_STATUS_MENU_H
#define GUARD_WIRELESS_STATUS_MENU_H

#include "engine.h"
#include "scene_input_widget.h"
#include "palette_resource.h"

struct WirelessStatusMenu {
    struct Task task;
    u8 state, entered;
    u8 unknown1A[2];
    u32 timer;
    s32 window;
    void *background, *messageTable;
    u32 completionProcedure;
    struct SceneInputResource resource;
    struct SceneInputWidget widget;
};

void TextWindowManager_DestroyAllWindows(void);
void PaletteLevels_Reset(void);
void SpriteOam_ResetQueue(int);
void Video_OnFrameNoOp(void);
void Video_OnRenderNoOp(void);
void sub_0821969C(void);
void Video_SetRenderCallbacks(int, void *, void *, void *);
void Hud_SetMode(int);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void Wireless_RequestDisconnectMode(void);
void Wireless_RequestResetConnection(void);
void Sound_FadeOutChannel10(int);
int TextWindow_Create(int, int, int, int);
void TextWindow_StartPlayback(int);
void TextWindow_Destroy(int);
int Wireless_GetStatus(void);
extern void *gUnk_08605770[];
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
void *Script_GetTextSourcePc(void);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
struct WirelessStatusMenu *WirelessStatusMenu_Get(void);
void WirelessStatusMenu_InitDisplay(struct WirelessStatusMenu *menu);
void WirelessStatusMenu_LoadPrimaryBackground(struct WirelessStatusMenu *menu, u32 index);
void WirelessStatusMenu_LoadStatusBackground(struct WirelessStatusMenu *menu, int layer);
void WirelessStatusMenu_UpdateIdleState(struct WirelessStatusMenu *menu);
void WirelessStatusMenu_UpdateDisconnectState(struct WirelessStatusMenu *menu);
void WirelessStatusMenu_UpdateDismissibleMessageState(struct WirelessStatusMenu *menu);
void WirelessStatusMenu_UpdateExitState(struct WirelessStatusMenu *menu);
int WirelessStatusMenu_Update(struct WirelessStatusMenu *menu);
int WirelessStatusMenu_Destroy(struct WirelessStatusMenu *menu);
int WirelessStatusMenu_InitFromScript(struct WirelessStatusMenu *menu);
struct WirelessStatusMenu *WirelessStatusMenu_Create(void);

#endif
