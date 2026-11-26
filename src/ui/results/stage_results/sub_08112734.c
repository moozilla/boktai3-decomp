#include "global.h"
void StageResults_DestroyTextWindows(void *);
void StageResults_DestroySprites(void *);
void CharacterModel_RemoveAnimations(void *);
u32 ScriptDisplay_Destroy(u8 *p)
{
    StageResults_DestroyTextWindows(p);
    StageResults_DestroySprites(p);
    CharacterModel_RemoveAnimations(p + 0xF60);
    return 1;
}
