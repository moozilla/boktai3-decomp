#include "global.h"
#include "game_state.h"
#define StateFlags_SetCategoryBit sub_0811DCC4
void sub_0811DCC4(u32 category, u32 bit)
{
    u32 mask = 1 << bit;
    switch (category) {
    case 0: gUnk_02000710->unk7B8[0] |= mask; break;
    case 1: gUnk_02000710->unk7B8[2] |= mask; break;
    case 2: gUnk_02000710->unk7B8[1] |= mask; break;
    case 3: gUnk_02000710->unk7B8[3] |= mask; break;
    case 4: gUnk_02000710->unk81C |= mask; break;
    case 5: gUnk_02000710->unk7F8 |= mask; break;
    }
}
