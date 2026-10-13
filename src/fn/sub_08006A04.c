#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
u32 Script_ParseStringRef(void *);
u8 *Text_LookupString(u32);
u8 *ChoiceMenu_ParseTag(struct Menu *, u8 *);
u16 sub_08032070(u8 *);
void ChoiceMenu_LayoutText(struct Menu *p, void *window)
{
    u8 *text = Text_LookupString(Script_ParseStringRef(window) + p->unknown60);
    s32 i;
    u8 *row, *rowStart, *rowEnd;
    s32 zero;
    p->count = 0;
    p->width = 0;
    zero = 0;
    rowStart = p->rowStart;
    rowEnd = p->rowEnd;
    row = rowStart;
    for (i = 7; i >= 0; i--) {
        row[0] = zero;
        row[8] = zero;
        row[16] = zero;
        row++;
    }
    p->rows = 0;
    for (;;) {
        if ((s8)*text < 0)
            goto doubleCharacter;
        if (!*text)
            break;
        if (*text == '<') {
            text++;
            if (!*text)
                break;
            text = ChoiceMenu_ParseTag(p, text);
            if (!*text)
                break;
        } else if (*text == '\n') {
            p->textX = 0;
            p->width = 0;
            if (p->rows < p->pad38[1]) {
                rowEnd[p->rows] = p->count ? p->count - 1 : 0;
                p->rows++;
                if (p->rows < p->pad38[1])
                    rowStart[p->rows] = p->count;
            }
            text++;
        } else {
            if (*text == 31) {
                u16 value = sub_08032070(text);
                if (value != 65535) {
                    if (({
                            u32 mask = 255;
                            asm("" : "+r"(mask));
                            mask <<= 8;
                            mask &= value;
                            mask;
                        }))
                        goto doubleCharacter;
                    p->width++;
                    p->textX++;
                    text += 2;
                    continue;
                }
            }
            p->width++;
            p->textX++;
            text++;
        }
        continue;
    doubleCharacter:
        p->width += 2;
        p->textX += 2;
        text += 2;
    }
    if (p->rows < p->pad38[1])
        rowEnd[p->rows] = p->count ? p->count - 1 : 0;
}
