#include "global.h"
#include "engine.h"
const u8 *Script_ReadString(const u8 *pc);
/* Script: copies the string operand -n (up to 11 bytes) into the 16-byte name
   at game state +0x790. */
void GameState_SetNameFromScript(void)
{
    if (Script_SeekToKeyword('n')) {
        const u8 *text;
        Memory_Clear((u8 *)gGameState + 0x790, 16);
        text = Script_ReadString(Script_GetPc());
        if (text) {
            s32 i = 0;
            u8 *name = (u8 *)gGameState + 0x790;
            do {
                u8 c = text[i];
                if (!c)
                    break;
                name[i] = c;
                i++;
            } while (i <= 10);
            name[i] = 0;
        }
    }
}
