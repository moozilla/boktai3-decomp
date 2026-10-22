#include "global.h"
s32 Script_SeekToKeyword(s32);
u32 Script_GetTextSourcePc(void);
s32 TextWindow_Create(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, u32);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_Clear(s32);
u8 *sub_08227E9C(void);
void Script_SetPc(u8 *);
s32 Script_GetValue(void);
s32 Script_GetKeywordValue(s32, s32);
void ScrollingDialogue_ClearScript(void);
u32 BestiaryDisplay_Create(u32, u32);
void BestiaryDisplay_Hide(void);
void MonsterBook_InitWidgets(u8 *);
void MonsterBook_SetPage(u8 *, u32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);

struct E {
    u8 a, b, c, d;
};
struct MonsterBook {
    u8 pad[0xDAC];
    struct E e[83];
};

s32 MonsterBook_Init(u8 *s)
{
    s32 i;
    s32 j;
    s32 y;
    s32 *win;
    u8 *src;
    u8 *d;

    *(u32 *)(s + 0x20) = 0;
    s[0x24] = 0;
    s[0x25] = 0;
    s[0x26] = 0;
    s[0x48] = 0;
    *(u16 *)(s + 0xD4C) = 0;
    s[0x49] = 0x20;
    s[0x4A] = 0;
    if (Script_SeekToKeyword('n') == 0)
        return -1;
    if ((*(u32 *)(s + 0xD50) = Script_GetTextSourcePc()) == 0)
        return -1;
    j = 0;
    win = (s32 *)(s + 0x28);
    y = 5;
    do {
        *win = TextWindow_Create(3, y, 0x17, 2);
        TextWindow_SetScript(*win, *(u32 *)(s + 0xD50));
        TextWindow_Refresh(*win, j);
        TextWindow_Clear(*win++);
        y += 2;
        j++;
    } while (j <= 7);
    if (Script_SeekToKeyword('d')) {
        Script_SetPc(sub_08227E9C());
        i = 0;
        d = s + 0xD54;
        for (; i <= 0x52; i++)
            d[i] = Script_GetValue();
    }
    if (Script_SeekToKeyword('t')) {
        src = sub_08227E9C();
        for (i = 0; i <= 0x52; i++) {
            ((struct MonsterBook *)s)->e[i].a = *src++;
            ((struct MonsterBook *)s)->e[i].b = *src++;
            ((struct MonsterBook *)s)->e[i].c = *src++;
            ((struct MonsterBook *)s)->e[i].d = *src++;
        }
    }
    if (Script_SeekToKeyword('i'))
        *(u32 *)(s + 0xEF8) = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('s'))
        *(u32 *)(s + 0xF00) = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('p'))
        *(u32 *)(s + 0xEFC) = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('w'))
        *(u32 *)(s + 0xF04) = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('A'))
        *(u32 *)(s + 0xF08) = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('a'))
        *(u32 *)(s + 0xF0C) = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('m'))
        *(u32 *)(s + 0xF10) = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('T'))
        *(u32 *)(s + 0xF14) = Script_GetTextSourcePc();
    *(s32 *)(s + 0xF18) = Script_GetKeywordValue('e', 0);
    {
        u16 *q = (u16 *)(s + 0xD4E);
        i = 0;
        *q = 5;
    }
    ScrollingDialogue_ClearScript();
    *(u32 *)(s + 0xF1C) = BestiaryDisplay_Create(0, 0);
    BestiaryDisplay_Hide();
    MonsterBook_InitWidgets(s);
    MonsterBook_SetPage(s, 0);
    ScreenTransition_SendCommand(0, 4, 4, 4, 4, 0xFFFF, i);
    Sound_PlaySong(0x118);
    return 0;
}
