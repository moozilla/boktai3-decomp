#ifndef GUARD_WIRELESS_EXIT_H
#define GUARD_WIRELESS_EXIT_H

#include "engine.h"

struct WirelessExit {
    struct Task task;
    u32 completionProcedure;
    u16 state;
};

int WirelessExit_Update(struct WirelessExit *exit);
u32 WirelessExit_OnDestroy(void);
int WirelessExit_Init(struct WirelessExit *exit);
struct WirelessExit *WirelessExit_Create(void);

void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void Wireless_RequestDestroyController(void);
void *WirelessExchange_Get(void);
int WirelessController_GetInstance(void);
void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
void Video_SetRenderCallbacks(u32, void *, void *, void *);
void Video_OnFrameNoOp(void);
void Video_OnRenderNoOp(void);
void Sprite_OnRenderPhase(void);

#endif
