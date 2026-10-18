// COMPILER: old_agbcc
#include "global.h"
/* SceneSpriteBurst_ResetSingleton: Clears the shared burst manager pointer */
#define SceneSpriteBurst_ResetSingleton sub_08121F70

extern u32 gUnk_02000138;

void sub_08121F70(void)
{
    gUnk_02000138 = 0;
}
