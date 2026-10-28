#include "global.h"
#include "ram.h"
#include "runtime.h"
#include "render.h"
extern u32 gUnk_020004B4;
void m4aMPlayAllStop(void);
void m4aSoundVSyncOff(void);
void InterruptsAndVideo_WaitForVBlank(void);
void Solar_StopMeasurement(void);
void Rtc_Protect(void);
void Serial_StopCommunication(void);
void rfu_setREQCallback(void (*callback)(u16, u16));
void rfu_REQ_stopMode(void);
u16 rfu_waitREQComplete(void);
u32 System_ShutdownAndSoftReset(void)
{
    gScanlineDmaSource = 0;
    gScanlineDmaTarget = 0;
    gScanlineDmaCount = 0;
    gScanlineDmaMode = 0;
    gVideoTransferControl = 0;
    m4aMPlayAllStop();
    m4aSoundVSyncOff();
    InterruptsAndVideo_WaitForVBlank();
    InterruptsAndVideo_WaitForVBlank();
    Solar_StopMeasurement();
    Rtc_Protect();
    if (gUnk_020004B4 || gSlotMonitor)
        Serial_StopCommunication();
    rfu_setREQCallback(0);
    rfu_REQ_stopMode();
    if (rfu_waitREQComplete() == 0)
        SoftReset(0xDC);
    else
        SoftReset(0xDC);
    return 0;
}
