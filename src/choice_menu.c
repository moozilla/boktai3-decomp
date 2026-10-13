#include "choice_menu.h"

struct Args {
    u32 count : 16;
    u32 unused : 16;
    u32 *values;
};

void sub_0821AD08(u32, struct Args *);

void sub_0824923C(s32, void *);

static inline u32 selectionScript(struct ChoiceMenu *p)
{
    return p->script;
}

void ChoiceMenu_DispatchSelection(struct ChoiceMenu *p, s32 selection)
{
    u32 values[5];
    struct Args args;
    if (p->useScript) {
        s32 i;
        u32 script;
        values[0] = selection;
        i = 0;
        for (; i < 4; i++)
            values[i + 1] = p->arguments[i];
        args.count = 5;
        args.values = values;
        script = selectionScript(p);
        if (script)
            sub_0821AD08(script, &args);
    } else
        sub_0824923C(selection, p->callback);
}

void sub_0822B2F8(s32);

void ChoiceMenu_Select(struct ChoiceMenu *p, s32 selection)
{
    struct Choice *choice;
    if (selection >= 0) {
        if (selection >= p->count)
            selection = p->count - 1;
        choice = &p->choices[selection];
        p->leftCursor.flags &= ~512;
        p->rightCursor.flags &= ~512;
        p->leftCursor.x = choice->x - 7;
        p->leftCursor.y = choice->y;
        p->rightCursor.x = choice->x + 1 + (choice->width << 3);
        p->rightCursor.y = choice->y;
    } else {
        choice = &p->choices[p->initial];
        p->leftCursor.flags |= 512;
        p->rightCursor.flags |= 512;
        p->leftCursor.x = choice->x - 7;
        p->leftCursor.y = choice->y;
        p->rightCursor.x = choice->x + 1 + (choice->width << 3);
        p->rightCursor.y = choice->y;
    }
    if (selection != p->selected)
        sub_0822B2F8(220);
    p->selected = selection;
}

s32 sub_08006478(struct ChoiceMenu *p, s32 a, s32 d)
{
    s32 x = a;
    s32 i;
    for (i = 0; i < p->rows; i++) {
        x += d;
        if (x > p->rows)
            x = p->rows;
        else if (x < 0)
            x = 0;
        if (p->rowCount[x] != 0)
            return x;
    }
    return a;
}

void sub_08030BF8(void);

void sub_0821A0C0(void *);

// MATCHING_CONCESSION: register-pinning
s32 ChoiceMenu_HandleInput(struct ChoiceMenu *p)
{
    p->leftCursor.flags &= ~1;
    p->rightCursor.flags &= ~1;
    if (p->selected < 0) {
        if ((gUnk_03005260.pressed & 240) || (!p->wakeDelay && (gUnk_03005260.pressed & 3))) {
            ChoiceMenu_Select(p, p->initial);
            sub_0822B2F8(263);
        } else if (p->wakeDelay)
            p->wakeDelay--;
    } else if (p->wakeDelay)
        p->wakeDelay--;
    else {
        register struct ChoiceKeys *initialKeys asm("r1") = &gUnk_03005260;
        struct ChoiceKeys *keys;
        u8 *repeat;
        u32 pressed;
        u32 sameDirection;
        sameDirection = ({
            u32 held = initialKeys->held;
            u32 mask = p->held;
            mask &= held;
            mask;
        });
        keys = initialKeys;
        if (sameDirection) {
            u8 *ptr = &p->repeatDelay;
            register u32 delay asm("r1") = *ptr;
            repeat = ptr;
            if (delay) {
                u32 value = delay - 1;
                {
                    register u8 *destination asm("r1") = repeat;
                    asm("" : "+r"(destination));
                    *destination = value;
                }
            }
        } else {
            p->repeatDelay = sameDirection;
            repeat = &p->repeatDelay;
        }
        p->held = keys->held & 240;
        pressed = keys->pressed;
        if (pressed & 2) {
            if (p->allowCancel) {
                sub_0822B2F8(222);
                sub_08030BF8();
                ChoiceMenu_DispatchSelection(p, -1);
                p->leftCursor.flags |= 1;
                p->rightCursor.flags |= 1;
                sub_0821A0C0(p);
                return 1;
            }
        } else {
            register u32 one asm("r4") = 1;
            if (({
                    u32 bits = one;
                    bits &= pressed;
                    asm("" : : "r"(bits));
                    bits;
                })) {
                sub_0822B2F8(221);
                sub_08030BF8();
                ChoiceMenu_DispatchSelection(p, p->selected);
                p->leftCursor.flags |= one;
                p->rightCursor.flags |= one;
                sub_0821A0C0(p);
                return 1;
            }
        }
        if (!({
                u8 *ptr = repeat;
                *ptr;
            })) {
            register s32 currentSelection asm("r2") = p->selected;
            struct Choice *choice = &p->choices[currentSelection];
            s32 selection, limit;
            u32 held = keys->held;
            if (held & 32) {
                selection = currentSelection - 1;
                limit = p->rowStart[choice->row];
                if (selection >= limit)
                    goto applySelection;
                else
                    goto clampSelection;
            } else if (held & 16) {
                selection = currentSelection + 1;
                limit = p->rowEnd[choice->row];
                if (selection > limit)
                    goto clampSelection;
            } else if (held & 64) {
                s32 column = currentSelection - ({
                                 u32 value = p->rowStart[choice->row];
                                 value;
                             });
                s32 row = sub_08006478(p, choice->row, -1);
                if (choice->row == row)
                    selection = p->selected;
                else
                    selection = p->rowStart[row] + column;
                limit = p->rowStart[row];
                if (selection < limit)
                    goto clampSelection;
                limit = p->rowEnd[row];
                if (selection > limit)
                    goto clampSelection;
            } else if (({
                           u32 bits = 128;
                           bits &= held;
                           bits;
                       })) {
                register s32 column asm("r6") = currentSelection - ({
                                                    register u32 value asm("r0") =
                                                        p->rowStart[choice->row];
                                                    value;
                                                });
                s32 row = sub_08006478(p, choice->row, 1);
                if (choice->row == row)
                    selection = p->selected;
                else
                    selection = p->rowStart[row] + column;
                limit = p->rowStart[row];
                if (selection < limit)
                    goto clampSelection;
                limit = p->rowEnd[row];
                if (selection > limit)
                    goto clampSelection;
            } else
                return 0;
            goto applySelection;
        clampSelection:
            selection = limit;
        applySelection:
            ChoiceMenu_Select(p, selection);
            {
                u32 delay = 30;
                *repeat = delay;
            }
        }
    }
    return 0;
}

s32 sub_08031E34(void *);

u32 sub_082282A4(u32);

u32 sub_080066C0(struct ChoiceMenu *p)
{
    if (p->window != 0 && sub_08031E34(p->window) != 0) {
        if (p->delay == 0) {
            if (ChoiceMenu_HandleInput(p) == 1)
                return 0;
        } else {
            p->delay--;
        }
        p->palette[12] = sub_082282A4(p->colorIndex);
        p->colorIndex = (p->colorIndex + 1) & 0x3f;
    }
    return 0;
}

void sub_082195E0(void *);

u32 sub_08006710(struct ChoiceMenu *p)
{
    sub_082195E0(&p->leftCursor);
    sub_082195E0(&p->rightCursor);
    return gUnk_0200002C = 0;
}

void sub_08030F20(u32, u32, u32, u32);

void sub_08030B78(u32);

void sub_08030C84(u32);

void sub_08030DBC(u32);

void sub_08006738(struct ChoiceMenu *s)
{
    sub_08030F20(s->left, s->top, s->maxWidth, s->maxRows);
    sub_08030B78(1);
    sub_08030C84((u32)s->window);
    sub_08030DBC(s->stringBase);
}

extern const u8 gUnk_0824DB0C[], gUnk_0824DB14[], gUnk_0824DB1C[], gUnk_0824DB20[];

s32 sub_08032168(const u8 *, const u8 *, s32);

s32 sub_080321A0(const u8 *, s32);

s32 sub_08031DDC(s32);

s32 sub_08031DBC(s32);

u8 *Text_FindChar(u8 *, s32);

// MATCHING_CONCESSION: register-pinning
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

u32 Script_ParseStringRef(void *);

u8 *Text_LookupString(u32);

u16 sub_08032070(u8 *);

// MATCHING_CONCESSION: register-pinning
void ChoiceMenu_LayoutText(struct ChoiceMenu *p, void *window)
{
    u8 *text = Text_LookupString(Script_ParseStringRef(window) + p->stringBase);
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
            if (p->rows < p->maxRows) {
                rowEnd[p->rows] = p->count ? p->count - 1 : 0;
                p->rows++;
                if (p->rows < p->maxRows)
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
    if (p->rows < p->maxRows)
        rowEnd[p->rows] = p->count ? p->count - 1 : 0;
}

struct Dimensions {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};

struct ChoiceResource *sub_0821A520(s32, s32);

void sub_082196C4(struct ChoiceResource *, struct ChoiceResource *);

void sub_0821983C(void *, struct ChoiceResource *, s32, s32, s32, s32, s32, struct Dimensions *);

void sub_08219EBC(void *, void *, s32);

s32 sub_08031E1C(void);

s32 ChoiceMenu_Initialize(struct ChoiceMenu *p, void *window, s32 stringBase, s32 *options)
{
    struct ChoiceResource *resource;
    struct Dimensions dimensions;
    s32 initial;
    u16 *palette;
    gUnk_0200002C = p;
    resource = sub_0821A520(0xCB05, 0x530D);
    if (!resource)
        return -1;
    p->resource = *resource;
    sub_082196C4(&p->resource, resource);
    p->window = window;
    p->stringBase = stringBase;
    if (options[1]) {
        p->selected = -1;
        initial = options[0];
    } else {
        initial = options[0];
        p->selected = initial;
    }
    if (initial < 0)
        initial = 0;
    p->initial = initial;
    p->allowCancel = options[2];
    p->wakeDelay = 10;
    p->repeatDelay = 0;
    p->held = gUnk_03005260.held;
    p->left = options[3];
    p->top = options[4];
    p->maxWidth = options[5];
    p->maxRows = options[6];
    p->delay = 2;
    dimensions.x = 1;
    dimensions.y = 0;
    dimensions.z = 0;
    sub_0821983C(&p->leftCursor, &p->resource, 73, 17, 0, 0, 60, &dimensions);
    sub_0821983C(&p->rightCursor, &p->resource, 74, 17, 0, 0, 60, &dimensions);
    palette = p->palette;
    sub_08219EBC(palette, p->leftCursor.palette, 32);
    p->leftCursor.palette = palette;
    p->rightCursor.palette = palette;
    if (p->window) {
        ChoiceMenu_LayoutText(p, p->window);
        ChoiceMenu_Select(p, p->selected);
        sub_08006738(p);
    }
    if (!sub_08031E1C())
        sub_0822B2F8(263);
    return 0;
}

#define ChoiceMenu_Create sub_08006CD0

void *sub_08219FBC(s32, s32);

struct ChoiceMenu *ChoiceMenu_Create(void *window, s32 stringBase, void *callback, s32 *options)
{
    struct ChoiceMenu *p;
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

s32 Script_SeekToKeyword(s32);

s32 Script_GetValue(void);

s32 Script_GetPc(void);

s32 sub_0821ABA8(s32, s32);

void *sub_08227E90(void);

s32 sub_08031D90(s32 *);

// MATCHING_CONCESSION: register-pinning
struct ChoiceMenu *ChoiceMenu_CreateFromScript(void)
{
    s32 options[7], arguments[4], defaults[4];
    void *window;
    s32 stringBase, script;
    struct ChoiceMenu *p;
    s32 i;
    s32 *argumentBase;
    if (gUnk_0200002C)
        return 0;
    if (Script_SeekToKeyword('r'))
        window = sub_08227E90();
    else
        window = 0;
    stringBase = sub_0821ABA8('i', 0);
    script = 0;
    if (Script_SeekToKeyword('p'))
        script = Script_GetValue();
    options[1] = sub_0821ABA8('l', 0);
    options[0] = sub_0821ABA8('w', 0);
    options[2] = sub_0821ABA8('c', 0);
    if (Script_SeekToKeyword('a')) {
        options[3] = Script_GetValue();
        options[4] = Script_GetValue();
        options[5] = Script_GetValue();
        options[6] = Script_GetValue();
    } else if (!sub_08031D90(defaults)) {
        options[3] = defaults[0];
        options[4] = defaults[1];
        options[5] = defaults[2];
        options[6] = defaults[3];
    } else {
        options[3] = 0;
        options[4] = 0;
        options[5] = 24;
        options[6] = 3;
    }
    if (Script_SeekToKeyword('A')) {
        s32 *arg;
        argumentBase = arguments;
        arg = argumentBase;
        do {
            s32 value = Script_GetPc();
            if (value)
                value = Script_GetValue();
            *arg = value;
            asm("" : : "r"(arg));
            arg++;
        } while ((s32)arg <= ({
                     register s32 limit asm("r0") = (s32)argumentBase;
                     limit + 12;
                 }));
    } else {
        s32 *arg;
        s32 zero;
        s32 *begin;
        argumentBase = arguments;
        begin = argumentBase;
        asm("" : "+r"(begin));
        zero = 0;
        arg = arguments + 3;
        do {
            *arg = zero;
            arg--;
        } while ((s32)arg >= (s32)begin);
    }
    p = sub_08219FBC(11, 0x1E8);
    if (p) {
        sub_0821A04C(p, sub_080066C0, sub_08006710);
        p->useScript = 1;
        p->script = script;
        {
            s32 *source = argumentBase;
            u32 *destination = p->arguments;
            for (i = 3; i >= 0; i--)
                *destination++ = *source++;
        }
        if (ChoiceMenu_Initialize(p, window, stringBase, options) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}

void sub_08006E98(void)
{
    struct ChoiceMenu *p = gUnk_0200002C;
    if (p != 0 && Script_SeekToKeyword(0x72)) {
        p->window = sub_08227E90();
        p->stringBase = sub_0821ABA8(0x69, 0);
        if (Script_SeekToKeyword(0x77))
            p->selected = Script_GetValue();
        if (p->window != 0) {
            ChoiceMenu_LayoutText(p, p->window);
            ChoiceMenu_Select(p, p->selected);
            sub_08006738(p);
        }
    }
}
