#include "global.h"
struct TextState {
    u8 x, y, left, top, width, height, palette, style;
    u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice, toggle, lastWidth,
        pad14;
    s8 current;
    u8 instant, wrap;
    u32 frame;
    const u8 *pc, *streams[4];
    u8 pad30[0xD8];
    u8 depth, styles[7];
};
struct TextWindow {
    s32 id;
    u8 x, y, width, height;
    u32 enabled;
    u16 clock, index;
    s16 symbol;
    u16 pad12;
    const u8 *script;
    u16 sequenceIndex, sequenceCount;
    u16 sequence[32];
    void *callback;
    struct TextState text;
    u8 pad170[0x54];
    struct TextWindow *next, *previous;
};
struct WindowManager {
    u8 pad0[0x14];
    void *script;
    s32 nextId;
    struct TextWindow *head;
};
extern struct WindowManager *gUnk_020000E4;
void *sub_08219C40(int);
void sub_08219DD8(void *, int);
void sub_08219D38(void *);
void sub_08218E5C(int, int, int, int);
void sub_08032C80(struct TextState *, int, int, int, int);
void sub_08032C54(struct TextState *, int, int, int, int);
void sub_08032C34(struct TextState *);
int Script_ParseStringRef(const void *);
const u8 *Text_LookupString(int);
void sub_08033AF0(struct WindowManager *, struct TextWindow *);
void sub_08033AF8(struct WindowManager *, struct TextWindow *);
struct TextWindow *sub_0803360C(struct WindowManager *, int);
struct TextWindow *sub_08033634(int);
int sub_08033654(struct WindowManager *);
const u8 *sub_080335F4(struct TextWindow *, int);
// MATCHING_CONCESSION: register-pinning
int sub_08033778(int id)
{
    struct WindowManager *p = gUnk_020000E4;
    struct TextWindow *w;
    struct TextWindow **nextSlot;
    struct TextWindow **prevSlot;
    struct TextWindow *next;
    int result;
    if (!p)
        goto fail;
    w = sub_0803360C(p, id);
    if (w)
        goto found;
fail:
    result = -1;
    goto done;
found:
    sub_08218E5C(w->x, w->y, w->width, w->height);
    w->callback = sub_08033AF0;
    {
        struct TextWindow **initial = &w->next;
        next = *initial;
        nextSlot = initial;
    }
    if (next) {
        u32 offset = 0x1C8;
        register struct TextWindow **link asm("r2");
        link = (struct TextWindow **)((u8 *)next + offset);
        prevSlot = (struct TextWindow **)((u8 *)w + offset);
        *link = *prevSlot;
    } else {
        prevSlot = &w->previous;
        p->head = *prevSlot;
    }
    {
        struct TextWindow **last = prevSlot;
        struct TextWindow *previous;
        asm("" : "+r"(last));
        previous = *last;
        if (previous)
            previous->next = *nextSlot;
        *nextSlot = 0;
        *last = 0;
    }
    sub_08219D38(w);
    result = id;
done:
    return result;
}
