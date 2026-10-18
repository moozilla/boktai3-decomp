#include "global.h"
#include "game_state.h"
u32 StateFlags_GetCategoryBit(u32 category, u32 bit)
{
    u32 mask = 1 << bit;
    switch (category) {
        case 0:
            return gUnk_02000710->unk7B8[0] & mask;
        case 1:
            return gUnk_02000710->unk7B8[2] & mask;
        case 2:
            return gUnk_02000710->unk7B8[1] & mask;
        case 3:
            return gUnk_02000710->unk7B8[3] & mask;
        case 4:
            return gUnk_02000710->unk81C & mask;
        case 5:
            return gUnk_02000710->unk7F8 & mask;
    }
    return 0;
}
