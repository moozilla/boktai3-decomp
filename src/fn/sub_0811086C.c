#include "global.h"
#include "ram.h"
void LinkScorePanel_RemoveSprites(u8 *);
void CharacterModel_RemoveAnimations(u8 *);
u32 SaveMenu_Destroy(u8 *p)
{
    LinkScorePanel_RemoveSprites(p);
    CharacterModel_RemoveAnimations(p + 0xD8C);
    gSaveMenu = 0;
    return 1;
}
