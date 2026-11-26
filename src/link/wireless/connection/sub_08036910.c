#include "engine/engine.h"
#include "link/wireless/wireless_controller.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "link/wireless/random.h"
void Solar_PauseMeasurement(void);
void Task_LinkToPriorityList(struct RuntimeTask *);
int Wireless_InitControllerFromScript(WirelessController *);
WirelessController *Wireless_CreateController(void)
{
    WirelessController *p;
    if (gWirelessController)
        return gWirelessController;
    Solar_PauseMeasurement();
    p = Heap_Alloc(sizeof(*p));
    if (!p)
        return 0;
    Memory_Clear(p, sizeof(*p));
    {
        int second = SunClock_GetSecond();
        u32 sample = RANDOM_NEXT();
        p->gameId = (gSceneEffectPalettePhase & 0xFF) | (((second + sample) & 0xFF) << 8);
    }
    if (Wireless_InitControllerFromScript(p) < 0) {
        Heap_Free(p);
        return 0;
    }
    Task_SetCallbacks(p, 0, 0);
    p->task.priority = 1;
    p->task.unk16 = 1;
    p->task.id = 0;
    Task_LinkToPriorityList(&p->task);
    gClockSynced = 1;
    gWirelessController = p;
    return p;
}
