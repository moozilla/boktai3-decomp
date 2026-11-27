#include "global.h"
#include "link/wireless/wireless_controller.h"
void Memory_Clear(void *, int);
#include "lib/rfu/rfu_compat.h"
#include "engine/ram.h"
#include "engine/input.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->operationStage = zero;
    p->operation = op;
    p->operationFrame = zero;
    p->operationEntered = 1;
}
void WirelessController_RebuildAvailablePeerList(struct Engine *);
int RfuManager_StartChildConnect(int, int);
int Wireless_SelectParentFromInput(struct Engine *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    WirelessController_RebuildAvailablePeerList(p);
    if (p->availablePeerCount) {
        int old = p->peerScroll;
        if (gPadState[1] & 64)
            p->peerScroll--;
        else if (gPadState[1] & 128)
            p->peerScroll++;
        if (old != p->peerScroll) {
            if (p->peerScroll > 7)
                p->peerScroll = 7;
            else if (p->peerScroll < 0)
                p->peerScroll = 0;
        }
        {
            int index = p->peerScroll;
            int n = p->availablePeerCount;
            if ((gPadState[1] & 1) && index < n) {
                int parent = p->availablePeerIndices[index];
                if (!(u8)RfuManager_StartChildConnect(gRfuLinkStatus->partner[parent].id, 0)) {
                    setOperation(p, 10, 0);
                    return 0;
                }
            }
        }
    }
}
