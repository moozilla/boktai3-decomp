#include "global.h"
#include "runtime.h"
/* Hardware startup, subsystem initialization, then the task scheduler loop. */
struct Dma {
    vu32 source, dest, control;
};
void RegisterRamReset(u32);
void Rtc_Initialize(void);
void Clock_ClearState(void);
void Clock_CheckRtcDate(void);
void m4aSoundInit(void);
void InterruptsAndVideo_Initialize(void);
void Eeprom_SetCapacity(u32);
void SceneLifecycle_InitFrameCounterPointer(void);
void GameState_SelectRamBase(void);
void TaskSystem_Initialize(void);
void InterruptsAndVideo_InitRuntime(void);
void Terrain_Init(void);
void AnimData_InitPalettePool(void);
void *ScriptProc_Start(u32, u32);
void SpriteTiles_LoadResource(void *);
void AnimData_SetResourceBank(void *);
void Serial_InitMultiplayer(void);
void Time_CalculateSunriseSunset(s32, s32, s32);
void Script_InitRuntime(void);
void SceneLifecycle_InitManager(void);
void TaskSystem_RunFrame(void);
extern u8 Palette_CacheObjectBankArm[], AgbMain__addr[];
void AgbMain(void)
{
    u32 saved0, saved1, zero;
    u32 *cookie0, *cookie1;
    *(vu16 *)0x04000204 = 0x4014;
    {
        u32 anchor = 0x0401fb14;
        u32 offset = anchor + 0xfdfe0c00;
        cookie0 = (u32 *)(anchor - offset);
        saved0 = *cookie0;
        anchor += 4;
        cookie1 = (u32 *)(anchor - offset);
        saved1 = *cookie1;
    }
    RegisterRamReset(1);
    *cookie0 = saved0;
    *cookie1 = saved1;
    {
        struct Dma *dma;
        zero = 0;
        dma = (struct Dma *)0x040000d4;
        dma->source = (u32)&zero;
        dma->dest = 0x03000000;
        dma->control = 0x85001f40;
        dma->control;
        while (dma->control & 0x80000000)
            ;
    }
    Rtc_Initialize();
    Clock_ClearState();
    Clock_CheckRtcDate();
    m4aSoundInit();
    InterruptsAndVideo_Initialize();
    CpuSet(Palette_CacheObjectBankArm, gIwramCode,
           (((s32)(AgbMain__addr - Palette_CacheObjectBankArm) / 4) & 0x1fffff) | 0x04000000);
    Eeprom_SetCapacity(64);
    SceneLifecycle_InitFrameCounterPointer();
    GameState_SelectRamBase();
    TaskSystem_Initialize();
    InterruptsAndVideo_InitRuntime();
    Terrain_Init();
    AnimData_InitPalettePool();
    SpriteTiles_LoadResource(ScriptProc_Start(0xceaa, 0x3002));
    AnimData_SetResourceBank(ScriptProc_Start(0x98f5, 0xff54));
    {
        vu16 *ime = (vu16 *)0x04000208;
        vu16 *ie;
        u16 flags;
        *ime = 0;
        ie = (vu16 *)0x04000200;
        flags = *ie;
        *ie = 0;
        *ie = 4 | flags;
        *ime = 1;
    }
    Serial_InitMultiplayer();
    Time_CalculateSunriseSunset(0x23a666, 0x8bc000, 0);
    Script_InitRuntime();
    SceneLifecycle_InitManager();
    for (;;)
        TaskSystem_RunFrame();
}
