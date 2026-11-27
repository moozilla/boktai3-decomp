#include "global.h"
#include "link/wireless/wireless_controller.h"
void Memory_Clear(void *, int);
#include "lib/rfu/rfu_compat.h"
#include "engine/ram.h"
#include "engine/runtime.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->operationStage = zero;
    p->operation = op;
    p->operationFrame = zero;
    p->operationEntered = 1;
}
void Solar_PauseMeasurement(void);
void Solar_ResumeMeasurement(void);
void Task_UnlinkFromPriorityList(void *);
void Heap_Free(void *);
int sub_082238F0(int);
void Wireless_UpdateConnectionSession(struct Engine *);
void Wireless_UpdateInactiveConnectionSession(struct Engine *);
void Wireless_UpdateParentUniSession(struct Engine *);
void Wireless_UpdateChildUniSession(struct Engine *);
void Wireless_UpdateParentNiSession(struct Engine *);
void Wireless_UpdateChildNiSession(struct Engine *);
int Wireless_UpdateControllerTask(struct Engine *p)
{
    Solar_PauseMeasurement();
    gWirelessLastStatus = p->status;
    switch (p->sessionMode) {
        case 0:
            if (p->isParent)
                Wireless_UpdateConnectionSession(p);
            else
                Wireless_UpdateInactiveConnectionSession(p);
            break;
        case 1:
            if (p->isParent)
                Wireless_UpdateParentUniSession(p);
            else
                Wireless_UpdateChildUniSession(p);
            break;
        case 2:
            if (p->isParent)
                Wireless_UpdateParentNiSession(p);
            else
                Wireless_UpdateChildNiSession(p);
            break;
    }
    if (p->resetRequested) {
        p->resetRequested = 0;
        sub_082238F0(1);
        setOperation(p, 0, 0);
    }
    if (p->shutdownRequested) {
        gClockSynced = 0;
        gWirelessController = 0;
        Task_UnlinkFromPriorityList(p);
        Heap_Free(p);
        Solar_ResumeMeasurement();
    } else {
        if ((u16)(p->operation - 9) <= 17)
            gSceneDialogActive = 1;
        p->operationFrame++;
        p->frameCounter++;
    }
    return 0;
}
