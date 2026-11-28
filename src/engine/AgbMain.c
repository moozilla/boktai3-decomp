#include "global.h"
#include "engine/runtime.h"
#include "gba/io_reg.h"
/* Hardware startup, subsystem initialization, then the task scheduler loop. */
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
    u32 saved0, saved1;
    u32 *cookie0, *cookie1;
    u32 anchor, offset;

    REG_WAITCNT = 0x4014;
    /* Keep the two words after the heap (0x0201F400 and 0x0201F404) across the RAM reset. */
    anchor = 0x0401fb14;
    offset = anchor + 0xfdfe0c00;
    cookie0 = (u32 *)(anchor - offset);
    saved0 = *cookie0;
    anchor += 4;
    cookie1 = (u32 *)(anchor - offset);
    saved1 = *cookie1;
    RegisterRamReset(1);
    *cookie0 = saved0;
    *cookie1 = saved1;
    DmaFill32(3, 0, (void *)IWRAM_START, 0x7d00);
    DmaWait(3);
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
    REG_IME = 0;
    {
        u16 flags = REG_IE;
        REG_IE = 0;
        REG_IE = flags | 4;
    }
    REG_IME = 1;
    Serial_InitMultiplayer();
    Time_CalculateSunriseSunset(0x23a666, 0x8bc000, 0);
    Script_InitRuntime();
    SceneLifecycle_InitManager();
    for (;;)
        TaskSystem_RunFrame();
}
