#include "global.h"

extern u32 gUnk_020001CC;
void Registry3C9F_DestroyTextWindows(u8 *);
void Registry3C9F_RemoveAnimations(u8 *);
void CharacterModel_RemoveAnimations(u8 *);

s32 Registry3C9F_OnDestroy(u8 *p)
{
    Registry3C9F_DestroyTextWindows(p);
    Registry3C9F_RemoveAnimations(p);
    CharacterModel_RemoveAnimations(p + 0xa8c);
    gUnk_020001CC = 0;
    return 1;
}
