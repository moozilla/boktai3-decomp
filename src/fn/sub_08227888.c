#include "global.h"
#include "engine.h"
u8 *PlayerName_GetBuffer(void);
const u8 *Script_ReadString(const u8 *pc);
/* Registry routine 0272: clears the 16-byte name buffer from PlayerName_GetBuffer and
   copies the string -n into it (at most 16 bytes), or writes the default
   four-character name (tentative: the player's name). */
void PlayerName_SetFromScript(void)
{
    u8 *name = PlayerName_GetBuffer();
    u32 zero = 0;
    CpuSet(&zero, name, 0x05000004);
    if (Script_SeekToKeyword('n')) {
        const u8 *text = Script_ReadString(Script_GetPc());
        if (text) {
            s32 i = 0;
            if (*text) {
                const u8 *src = text;
                do {
                    name[i] = *src;
                    src++;
                    i++;
                    if (!*src)
                        break;
                } while (i <= 15);
            }
        }
    } else {
        name[0] = 0x80;
        name[1] = 0xA2;
        name[2] = 0x80;
        name[3] = 0x98;
        name[4] = 0x80;
        name[5] = 0x91;
        name[6] = 0x80;
        name[7] = 0x9F;
    }
}
