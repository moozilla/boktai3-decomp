#include "global.h"

#include "engine.h"
#include "ram.h"
extern u8 *gUnk_02000580[];
extern const u16 gUnk_08610E24[];
void sub_0813D834(u8 *, s32, s32);
/* Script: value -t (default gUnk_08610E24[-s]) for slot -s (0 or 1) of the
   player (sub_0813D834), or kept in gPlayerInitialSlotValues when there is no player
   yet (tentative). */
void Player_SetSlotValueFromScript(void)
{
    s32 n, value;
    if (Script_SeekToKeyword('s') && (n = Script_GetValue()) <= 1) {
        if (Script_SeekToKeyword('t'))
            value = Script_GetValue();
        else
            value = gUnk_08610E24[n];
        if (gUnk_02000580[0])
            sub_0813D834(gUnk_02000580[0], n, value);
        else
            gPlayerInitialSlotValues[n] = value;
    }
}
