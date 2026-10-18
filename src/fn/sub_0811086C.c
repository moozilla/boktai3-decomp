#include "global.h"
void LinkScorePanel_RemoveSprites(u8 *);
void CharacterModel_RemoveAnimations(u8 *);
extern u32 gUnk_020001D4;
u32 SaveMenu_Destroy(u8 *p)
{
    LinkScorePanel_RemoveSprites(p);
    CharacterModel_RemoveAnimations(p + 0xD8C);
    gUnk_020001D4 = 0;
    return 1;
}
