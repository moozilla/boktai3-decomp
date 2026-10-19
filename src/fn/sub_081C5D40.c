
#include "global.h"

struct SceneTextSelector;

void ChallengeMenu_InitAnimations(u8 *s);
void ChallengeMenu_LoadPalette(u8 *p);
void ChallengeMenu_SetStoredRank(s32 a, s32 b, u32 c);
void ChallengeMenu_StartFadeIn(u8 *p);
void SceneTextSelector_UpdatePanel(struct SceneTextSelector *p);
void sub_081C54B8(u8 *s);
void sub_081C5A10(u8 *s);
s32 Script_GetKeywordValue(u32, s32);
u32 Script_GetTextSourcePc(void);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);

struct ChallengeMenu {
    u8 unknown0[0x2e3];
    u8 enabledChoices;
    u8 unknown2e4[0x2e8 - 0x2e4];
    s32 ranks[4];
    s32 unk2f8;
    s32 group;
    s32 choice;
    const void *textRef;
    const void *textRef2;
    s32 unk30c;
    s32 unk310;
};

s32 ChallengeMenu_ScriptOpen(struct ChallengeMenu *s)
{
    if (Script_SeekToKeyword('e')) {
        s32 *out = s->ranks;
        s32 i = 3;
        do {
            *out++ = Script_GetValue();
        } while (--i >= 0);
    }
    s->enabledChoices = Script_GetKeywordValue('o', 7);
    if (Script_SeekToKeyword('s')) {
        s->group = Script_GetValue();
    }
    if (Script_SeekToKeyword('c')) {
        s->unk2f8 = Script_GetValue();
    } else {
        s->unk2f8 = 0;
    }
    if (Script_SeekToKeyword('m')) {
        s->textRef = (const void *)Script_GetTextSourcePc();
    }
    if (Script_SeekToKeyword('n')) {
        s->textRef2 = (const void *)Script_GetTextSourcePc();
    }
    if (Script_SeekToKeyword('A')) {
        s32 i = 0;
        u8 *base = (u8 *)s + 0xbf * 4;
        for (; i <= 2; i++) {
            s32 value = Script_GetValue();
            ChallengeMenu_SetStoredRank(*(s32 *)base, i, value);
        }
    }
    s->unk30c = 0;
    s->unk310 = 0;
    sub_081C5A10((u8 *)s);
    ChallengeMenu_LoadPalette((u8 *)s);
    ChallengeMenu_InitAnimations((u8 *)s);
    SceneTextSelector_UpdatePanel((struct SceneTextSelector *)s);
    sub_081C54B8((u8 *)s);
    ChallengeMenu_StartFadeIn((u8 *)s);
    s->choice = 0;
    return 0;
}
