// COMPILER: old_agbcc
#include "global.h"
/* TetherActor_GetSavedKind: Returns the saved kind at GameState offset 0x5AA as a signed halfword */
#define TetherActor_GetSavedKind sub_0804A400

extern u8 *gUnk_02000710;

s16 sub_0804A400(void)
{
    return *(s16 *)(gUnk_02000710 + 0x5aa);
}
