#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0813B3AC(u8 *, s32);
u32 Player_GetEventFlags(u8 *, u32);
u8 Player_IsEnchantmentAvailable(u8 *, s32);

void sub_0815A8A0(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    u8 *p = actor;
    if (actor != NULL && Script_SeekToKeyword('f')) {
        sub_0813B3AC(actor, Script_GetValue());
        if (Player_GetEventFlags(actor, 0x3ffe))
            actor[0x33d] = Player_IsEnchantmentAvailable(actor, (s8)actor[0x33c]);
        if (Script_SeekToKeyword('p'))
            *(u32 *)(p + 0xb38) = Script_GetValue();
    }
}
