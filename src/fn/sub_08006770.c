#include "choice_menu.h"
// MATCHING_CONCESSION: register-pinning
extern const u8 gUnk_0824DB0C[], gUnk_0824DB14[], gUnk_0824DB1C[], gUnk_0824DB20[];
s32 sub_08032168(const u8 *, const u8 *, s32);
s32 sub_080321A0(const u8 *, s32);
s32 sub_08031DDC(s32);
s32 sub_08031DBC(s32);
u8 *Text_FindChar(u8 *, s32);
u8 *ChoiceMenu_ParseTag(struct ChoiceMenu *p, u8 *text)
{
    u8 *cursor = text;
    if (*text != '/') {
        if (!sub_08032168(text, gUnk_0824DB0C, 5)) {
            READ_WIDTH();
            cursor = Text_FindChar(cursor, '>');
            p->choices[p->count].x = (p->left + ({
                                          register u8 *x asm("r4") = &p->textX;
                                          asm("" : : "r"(x));
                                          *x;
                                      }))
                                     << 3;
            p->choices[p->count].y = (p->top << 3) + (p->rows << 4);
            p->choices[p->count].row = p->rows;
            p->width = 0;
        } else if (!sub_08032168(text, gUnk_0824DB14, 6)) {
            register s32 width asm("r1") = sub_08031DDC(READ_NUMBER());
            p->width += width;
            p->textX += width;
            cursor = Text_FindChar(cursor, '>');
        } else if (!sub_08032168(text, gUnk_0824DB1C, 3)) {
            register s32 width asm("r1") = sub_08031DBC(READ_NUMBER());
            p->width += width;
            p->textX += width;
            cursor = Text_FindChar(cursor, '>');
        } else
            goto unknown;
    } else if (!sub_08032168(text, gUnk_0824DB20, 6)) {
        cursor = Text_FindChar(text, '>');
        if (p->explicitWidth)
            p->choices[p->count].width = p->explicitWidth;
        else
            p->choices[p->count].width = p->width;
        p->count++;
        p->width = 0;
        p->rowCount[p->rows]++;
    } else
        goto unknown;
    goto matchedTag;
unknown: {
    u8 *result = Text_FindChar(text, '>');
    register u8 *saved asm("r4") = result;
    asm("" : : "r"(saved));
    return result;
}
matchedTag:
    text = cursor + 1;
returnText:
    return text;
}
