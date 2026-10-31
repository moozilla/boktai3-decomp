#ifndef GUARD_CHOICE_MENU_H
#define GUARD_CHOICE_MENU_H

#include "engine.h"

/* One selectable entry; x/y are pixel positions, width in characters. */
struct Choice {
    u8 x, y, width, row;
};

/* 8-byte-header sprite object used for the left/right cursor arrows (0x60 bytes). */
struct ChoiceCursor {
    u8 head[8];
    u32 flags; // bit 0 hide, bit 9 hidden/idle
    u8 pad0C[20];
    s16 x, y;
    u8 pad24[36];
    u16 *palette;
    u8 pad4C[20];
};

struct ChoiceResource {
    u32 words[8];
};

struct ChoiceMenu {
    u8 task[24];
    s8 selected, initial;
    u8 delay, count;
    u8 rowStart[8], rowEnd[8], rowCount[8];
    u8 rows, useScript, left, top;
    u8 maxWidth, maxRows;
    u8 width, textX, allowCancel, wakeDelay, repeatDelay, explicitWidth;
    u32 held;
    u32 arguments[4];
    u32 script;
    void *callback, *window;
    u32 stringBase;
    struct Choice choices[16];
    u8 scratch[64];
    struct ChoiceResource resource;
    struct ChoiceCursor leftCursor;
    struct ChoiceCursor rightCursor;
    u32 colorIndex;
    u16 palette[16];
};

struct ChoiceKeys {
    u16 held, pressed;
};

/* 0x0200002C */
extern struct ChoiceMenu *gChoiceMenu;

void ChoiceMenu_DispatchSelection(struct ChoiceMenu *p, s32 selection);
void ChoiceMenu_Select(struct ChoiceMenu *p, s32 selection);
s32 ChoiceMenu_FindSelectableRow(struct ChoiceMenu *p, s32 a, s32 d);
s32 ChoiceMenu_HandleInput(struct ChoiceMenu *p);
u32 ChoiceMenu_Update(struct ChoiceMenu *p);
u32 ChoiceMenu_Destroy(struct ChoiceMenu *p);
void ChoiceMenu_PrintText(struct ChoiceMenu *p);
u8 *ChoiceMenu_ParseTag(struct ChoiceMenu *p, u8 *text);
void ChoiceMenu_LayoutText(struct ChoiceMenu *p, void *window);
s32 ChoiceMenu_Initialize(struct ChoiceMenu *p, void *window, s32 stringBase, s32 *options);
struct ChoiceMenu *sub_08006CD0(void *window, s32 stringBase, void *callback, s32 *options);
struct ChoiceMenu *ChoiceMenu_CreateFromScript(void);
void ChoiceMenu_ReloadFromScript(void);

/* Tag-parsing helpers; expand inside ChoiceMenu_ParseTag (use its p and cursor). */
#define READ_NUMBER()                                                                              \
    ({                                                                                             \
        s32 found = 0, length = 0, value;                                                          \
        while (*cursor != '>') {                                                                   \
            if (!*cursor) {                                                                        \
                cursor++;                                                                          \
                break;                                                                             \
            }                                                                                      \
            if (*cursor == '=')                                                                    \
                found = ({                                                                         \
                    register s32 one asm("r4") = 1;                                                \
                    one;                                                                           \
                });                                                                                \
            else if (({                                                                            \
                         register s32 f asm("r0") = found;                                         \
                         f;                                                                        \
                     }))                                                                           \
                p->scratch[length++] = *cursor;                                                    \
            cursor++;                                                                              \
        }                                                                                          \
        p->scratch[length] = 0;                                                                    \
        if (({                                                                                     \
                register s32 f asm("r1") = found;                                                  \
                f;                                                                                 \
            }) &&                                                                                  \
            length)                                                                                \
            value = Text_ParseDecimal(p->scratch, length);                                         \
        else                                                                                       \
            value = 0;                                                                             \
        value;                                                                                     \
    })
#define READ_WIDTH()                                                                               \
    ({                                                                                             \
        s32 found = 0, length = 0;                                                                 \
        while (*cursor != '>') {                                                                   \
            if (!*cursor) {                                                                        \
                cursor++;                                                                          \
                break;                                                                             \
            }                                                                                      \
            if (*cursor == '=')                                                                    \
                found = ({                                                                         \
                    register s32 one asm("r4") = 1;                                                \
                    one;                                                                           \
                });                                                                                \
            else if (({                                                                            \
                         register s32 f asm("r0") = found;                                         \
                         f;                                                                        \
                     }))                                                                           \
                p->scratch[length++] = *cursor;                                                    \
            cursor++;                                                                              \
        }                                                                                          \
        p->scratch[length] = 0;                                                                    \
        if (({                                                                                     \
                s32 f = found;                                                                     \
                f;                                                                                 \
            }) &&                                                                                  \
            length) {                                                                              \
            s32 value = Text_ParseDecimal(p->scratch, length);                                     \
            register u8 *out asm("r2") = &p->explicitWidth;                                        \
            asm("" : : "r"(out));                                                                  \
            *out = value;                                                                          \
        } else {                                                                                   \
            s32 zero = 0;                                                                          \
            register u8 *out asm("r3");                                                            \
            out = &p->explicitWidth;                                                               \
            *out = zero;                                                                           \
        }                                                                                          \
    })

#endif
