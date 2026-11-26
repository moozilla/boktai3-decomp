#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "engine/shared/wireless_controller.h"

s32 Wireless_InitRfuApi(void);
void WirelessController_ResetSession(WirelessController *);
void *Script_GetTextSourcePc(void);

s32 Wireless_InitControllerFromScript(WirelessController *p)
{
    p->status = 0;
    gWirelessLastStatus = 0;
    if (Wireless_InitRfuApi() < 0)
        return -1;

    p->shutdownRequested = 0;
    WirelessController_ResetSession(p);
    WirelessController_SetOperation(p, 1);

    p->connectionScript = (void *)Script_GetKeywordValue('p', 0);
    if (!Script_SeekToKeyword('a') || !(p->gameNameScript = Script_GetTextSourcePc()))
        return -1;
    return 0;
}
