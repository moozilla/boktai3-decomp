#include "global.h"

extern u8 *gUnk_02000710;

void sub_0811D384(u8 *);
void CharacterModel_SetComponentKeyframes(u8 *, u8 *);
void CharacterModel_LoadSecondaryPalette(u8 *, s32);

void ModifierMenu_RefreshPreviewModel(u8 *p)
{
    u8 *a = p + 0x83C;
    u8 *b = p + 0xE34;
    sub_0811D384(p + 0x884);
    CharacterModel_SetComponentKeyframes(a, b);
    CharacterModel_LoadSecondaryPalette(b, *(s16 *)(gUnk_02000710 + 0x81A));
}
