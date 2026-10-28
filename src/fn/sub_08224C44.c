#include "global.h"
#define gRfuLinkStatus gUnk_03006A80
#include "rfu_compat.h"
#include "rfu_manager.h"
#define G (&gRfuLinkManager)
struct RfuLinkManager;
void RfuManager_NotifyEvent(u8, u8);
void RfuManager_DisconnectAndWait(u8);

u8 sub_08224C44(u8 enable)
{
    if (G->state == 9 || G->state == 10 || G->state == 11) {
        G->event.words.code = 7;
        RfuManager_NotifyEvent(0xf3, 1);
        return 7;
    }
    if (enable)
        G->fastSearchParent_flag = 1;
    else
        G->fastSearchParent_flag = 0;
    return 0;
}
