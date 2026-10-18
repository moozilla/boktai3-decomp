#include "global.h"
void GameState_SetResultIndex(s32);
void CharacterModel_UpdatePairedAnimations(u8 *, s32);
void PaletteFade_CycleMenuColors(u16 *);
void Palette_UpdateHighlightPulse(u16 *, u8 *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void SelectionPanel_RunSelectionScript(u8 *);
void SelectionPanel_UpdateConfirmTransition(u8 *p)
{
    u8 *flag = p + 0xd2c;
    if (*flag) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xffff, 0);
        GameState_SetResultIndex((s8)p[0xd24] + *(s8 *)(p + 0xd26) * 6);
        *(u16 *)(p + 0xd2e) = 0;
        *flag = 0;
    }
    CharacterModel_UpdatePairedAnimations(p + 0xa8c, 0);
    PaletteFade_CycleMenuColors((u16 *)(p + 0x470));
    Palette_UpdateHighlightPulse((u16 *)(p + 0x468), p + 0xd07);
    if (*(u16 *)(p + 0xd2e) <= 31)
        ++*(u16 *)(p + 0xd2e);
    else
        SelectionPanel_RunSelectionScript(p);
}
