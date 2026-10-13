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
    u16 clock, index, symbol, pad12;
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
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
void Heap_Free(void *);
void TextWindow_BlankRect(int, int, int, int);
void sub_08032C80(struct TextState *, int, int, int, int);
void TextState_SetBounds(struct TextState *, int, int, int, int);
void TextState_Reset(struct TextState *);
int Script_ParseStringRef(const void *);
const u8 *Text_LookupString(int);
void sub_08033AF0(void);
void sub_08033AF8(void);
struct TextWindow *sub_0803360C(struct WindowManager *, int);
struct TextWindow *TextWindow_Find(int);
int sub_08033654(struct WindowManager *);
const u8 *sub_080335F4(struct TextWindow *, int);
int sub_08032CD4(struct TextState *, int, int);
int sub_08033A38(int id, int index, int value)
{
    struct TextWindow *w = TextWindow_Find(id);
    int result;
    if (!w)
        goto fail;
    return sub_08032CD4(&w->text, index, value);
fail:
    result = -1;
    return result;
}
