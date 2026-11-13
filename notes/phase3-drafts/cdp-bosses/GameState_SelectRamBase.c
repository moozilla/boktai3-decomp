#include "script_virtual_machine.h"
#include "ram.h"

void GameState_SelectRamBase(void)
{
    u32 timer = *(vu16 *)0x0400010C;
    u8 *base = gGameStateStorage;
    base += (timer & 127) * 8;
    gGameState = (struct GameState *)base;
    gBackupGameState = base + 0xb00;
    gGameVariables = base + 0x1600;
    gBackupGameVariables = base + 0x1a00;
    gScriptLocalVariables = base + 0x1e00;
}
