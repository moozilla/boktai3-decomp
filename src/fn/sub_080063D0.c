#include "global.h"
struct Choice {
    u8 x, y, width, row;
};
struct Menu {
    u8 task[24];
    s8 selected, initial;
    u8 delay, count;
    u8 rowStart[8], rowEnd[8], rowCount[8];
    u8 rows, useScript, left, top;
    u8 pad38[2];
    u8 width, textX, allowCancel, wakeDelay, repeatDelay, explicitWidth;
    u32 held;
    u32 arguments[4];
    u32 script;
    void *callback, *window;
    u32 unknown60;
    struct Choice choices[16];
    u8 scratch[104];
    u32 leftFlags;
    u8 pad110[20];
    s16 leftX, leftY;
    u8 pad128[68];
    u32 rightFlags;
    u8 pad170[20];
    s16 rightX, rightY;
};
void sub_0822B2F8(s32);
void ChoiceMenu_Select(struct Menu *p, s32 selection)
{
    struct Choice *choice;
    if (selection >= 0) {
        if (selection >= p->count)
            selection = p->count - 1;
        choice = &p->choices[selection];
        p->leftFlags &= ~512;
        p->rightFlags &= ~512;
        p->leftX = choice->x - 7;
        p->leftY = choice->y;
        p->rightX = choice->x + 1 + (choice->width << 3);
        p->rightY = choice->y;
    } else {
        choice = &p->choices[p->initial];
        p->leftFlags |= 512;
        p->rightFlags |= 512;
        p->leftX = choice->x - 7;
        p->leftY = choice->y;
        p->rightX = choice->x + 1 + (choice->width << 3);
        p->rightY = choice->y;
    }
    if (selection != p->selected)
        sub_0822B2F8(220);
    p->selected = selection;
}
