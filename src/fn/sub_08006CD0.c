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
#define ChoiceMenu_Create sub_08006CD0
extern struct Menu *gUnk_0200002C;
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
void sub_080066C0(void);
void sub_08006710(void);
void sub_0821A0C0(void *);
s32 ChoiceMenu_Initialize(struct Menu *, void *, s32, s32 *);
struct Menu *ChoiceMenu_Create(void *window, s32 stringBase, void *callback, s32 *options)
{
    struct Menu *p;
    if (gUnk_0200002C)
        return 0;
    p = sub_08219FBC(11, 0x1E8);
    if (p) {
        sub_0821A04C(p, sub_080066C0, sub_08006710);
        p->useScript = 0;
        p->callback = callback;
        if (ChoiceMenu_Initialize(p, window, stringBase, options) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
