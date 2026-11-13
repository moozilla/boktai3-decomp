#include "wireless_exit.h"

int WirelessExit_Update(struct WirelessExit *exit)
{
    switch (exit->state) {
        case 0:
            ScreenTransition_SendCommand(4, 0, 0, 0, 0, 65535, 0);
            Wireless_RequestDestroyController();
            {
                void *q = WirelessExchange_Get();
                if (q)
                    Task_Kill(q);
            }
            exit->state++;
            break;
        case 1:
            if (!WirelessController_GetInstance())
                exit->state++;
            break;
        case 2:
            if (exit->completionProcedure)
                Script_Call(exit->completionProcedure, 0);
            Task_Kill(exit);
            break;
    }
    return 0;
}

u32 WirelessExit_OnDestroy(void)
{
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    return 0;
}

int WirelessExit_Init(struct WirelessExit *exit)
{
    exit->completionProcedure = Script_GetKeywordValue('e', 0);
    exit->state = 0;
    Video_SetRenderCallbacks(0, Video_OnFrameNoOp, Video_OnRenderNoOp, Sprite_OnRenderPhase);
    return 0;
}

struct WirelessExit *WirelessExit_Create(void)
{
    struct WirelessExit *exit;
    exit = Task_Create(3, 0x20);
    if (exit != 0) {
        Task_SetCallbacks(exit, (void (*)(void *))WirelessExit_Update,
                          (void (*)(void *))WirelessExit_OnDestroy);
        if (WirelessExit_Init(exit) < 0) {
            Task_Kill(exit);
            return 0;
        }
    }
    return exit;
}
