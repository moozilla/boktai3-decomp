#include "engine.h"
#include "wireless_controller.h"
#include "ram.h"
#include "engine_globals.h"
void Solar_PauseMeasurement(void);
void Task_LinkToPriorityList(struct RuntimeTask *);
int Wireless_InitControllerFromScript(WirelessController *);
WirelessController *Wireless_CreateController(void)
{
    WirelessController **singleton = &gWirelessController;
    WirelessController *existing = *singleton;
    WirelessController *p;
    int random;
    if (existing)
        return existing;
    Solar_PauseMeasurement();
    p = Heap_Alloc(sizeof(*p));
    if (!p)
        return 0;
    Memory_Clear(p, sizeof(*p));
    random = SunClock_GetSecond();
    {
        u32 *position = &gRandomIndex;
        u32 index = *position + 1;
        const u16 *table;
        int term;
        const u8 *saltAddress;
        int salt;
        int byteMask;
        u16 *destination;
        index &= 0x3ff;
        *position = index;
        index <<= 1;
        table = gRandomTable;
        term = *(const u16 *)(index + (u32)table);
        saltAddress = &gSceneEffectPalettePhase;
        byteMask = 255;
        salt = *saltAddress;
        random += term;
        random &= byteMask;
        random <<= 8;
        salt |= random;
        destination = &p->gameId;
        *destination = salt;
    }
    if (Wireless_InitControllerFromScript(p) < 0) {
        Heap_Free(p);
        return 0;
    }
    {
        Task_SetCallbacks(p, 0, 0);
        p->task.priority = 1;
        p->task.unk16 = 1;
        p->task.id = 0;
        Task_LinkToPriorityList(&p->task);
        gClockSynced = 1;
        *singleton = p;
        return p;
    }
}
