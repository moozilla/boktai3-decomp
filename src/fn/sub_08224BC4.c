#include "global.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
#include "rfu_manager.h"
#define G (&gRfuLinkManager)
struct RfuLinkManager;
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);

u8 sub_08224BC4(u8 enable, u16 period)
{
    u16 ime;
    if (G->linkRecovery_enable && enable == 0 && G->linkRecoveryTimer.active)
        return 5;
    ime = REG_IME;
    REG_IME = 0;
    G->linkRecovery_enable = enable;
    G->linkRecoveryTimer.count_max = period;
    REG_IME = ime;
    return 0;
}
