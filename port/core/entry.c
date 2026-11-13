/* Cooperative counterpart of AgbMain. The compiler and entry points in src/
 * remain the ROM build's inputs. This file is linked only by the source port. */
#include "platform.h"
#include "boot.h"
#include "devices.h"
#include "hw.h"

void Clock_CheckRtcDate(void);
void m4aSoundInit(void);
void InterruptsAndVideo_Initialize(void);
void Eeprom_SetCapacity(unsigned);
void SceneLifecycle_InitFrameCounterPointer(void);
void GameState_SelectRamBase(void);
void TaskSystem_Initialize(void);
void InterruptsAndVideo_InitRuntime(void);
void Terrain_Init(void);
void AnimData_InitPalettePool(void);
void *ScriptProc_Start(unsigned, unsigned);
void SpriteTiles_LoadResource(void *);
void AnimData_SetResourceBank(void *);
void Serial_InitMultiplayer(void);
void Time_CalculateSunriseSunset(int, int, int);
void Script_InitRuntime(void);
void SceneLifecycle_InitManager(void);
void TaskSystem_RunFrame(void);
static int initialized;
static int last_error;

void port_system_call(unsigned number) { hw_system_call(number); }
void IntrMain(void) { last_error = port_irq_dispatch(); }
void InterruptsAndVideo_WaitForVBlank(void) { last_error = port_wait_vblank(); }

int port_game_initialize(void)
{
    uint16_t *ie = (uint16_t *)(uintptr_t)0x04000200;
    volatile uint16_t *ime = (volatile uint16_t *)(uintptr_t)0x04000208;
    if (initialized) return -1;
    port_devices_init(NULL, NULL);
    if (port_boot_early() < 0) return -1;
    Clock_CheckRtcDate();
    m4aSoundInit();
    InterruptsAndVideo_Initialize();
    if (port_boot_copy_iwram() < 0) return -1;
    Eeprom_SetCapacity(64);
    SceneLifecycle_InitFrameCounterPointer();
    GameState_SelectRamBase();
    TaskSystem_Initialize();
    InterruptsAndVideo_InitRuntime();
    Terrain_Init();
    AnimData_InitPalettePool();
    SpriteTiles_LoadResource(ScriptProc_Start(0xceaa, 0x3002));
    AnimData_SetResourceBank(ScriptProc_Start(0x98f5, 0xff54));
    *ime = 0;
    *ie |= 4;
    *ime = 1;
    Serial_InitMultiplayer();
    Time_CalculateSunriseSunset(0x23a666, 0x8bc000, 0);
    Script_InitRuntime();
    SceneLifecycle_InitManager();
    initialized = 1;
    last_error = 0;
    return 0;
}

int port_game_step(void)
{
    if (!initialized) return -1;
    last_error = 0;
    TaskSystem_RunFrame();
    return last_error < 0 ? last_error : 0;
}
