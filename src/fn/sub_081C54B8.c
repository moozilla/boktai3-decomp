#include "global.h"
#include "engine.h"
#define ChallengeMenu_DrawOptions sub_081C54B8
struct M {
    u8 pad[0x2E3];
    u8 mask;
    u8 pad2E4[0x2FC - 0x2E4];
    s32 sel;
};
void Menu_EraseRect(int, int, int, int);
void Menu_DrawText(int, int, void *);
void ChallengeMenu_DrawOptions(struct M *p)
{
    s32 base;
    s32 i;
    s32 y;
    base = Script_ParseStringRef(*(void **)((u8 *)p + 0x308));
    Menu_EraseRect(3, 6, 0x13, 2);
    Menu_DrawText(3, 6, Text_LookupString(base + 0xF));
    i = 0;
    y = 9;
    do {
        Menu_EraseRect(3, y, 0x13, 2);
        if ((p->mask >> i) & 1)
            Menu_DrawText(3, y, Text_LookupString(base + i + p->sel * 3));
        else
            Menu_DrawText(3, y, Text_LookupString(base + 0x10));
        y += 2;
        i++;
    } while (i <= 2);
}
