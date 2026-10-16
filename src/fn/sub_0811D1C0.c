#include "global.h"
#include "engine.h"
const u8 *sub_0821AAB0(const u8 *pc);
/* Script: copies the string operand -n (up to 11 bytes) into the 16-byte name
   at game state +0x790. */
void GameState_SetNameFromScript(void)
{
    if (Script_SeekToKeyword('n')) {
        const u8 *text;
        Memory_Clear((u8 *)gUnk_02000710 + 0x790, 16);
        text = sub_0821AAB0(Script_GetPc());
        if (text) {
            s32 i = 0;
            u8 *name = (u8 *)gUnk_02000710 + 0x790;
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
