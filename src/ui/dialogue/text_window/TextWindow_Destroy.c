#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_state.h"
#include "ui/dialogue/text_window_manager.h"
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
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
void Heap_Free(void *);
void TextWindow_BlankRect(int, int, int, int);
void TextState_Init(struct TextState *, int, int, int, int);
void TextState_SetBounds(struct TextState *, int, int, int, int);
void TextState_Reset(struct TextState *);
int Script_ParseStringRef(const void *);
const u8 *Text_LookupString(int);
void TextWindow_OnIdle(struct WindowManager *, struct TextWindow *);
void TextWindow_OnPlayback(struct WindowManager *, struct TextWindow *);
struct TextWindow *TextWindowManager_FindWindow(struct WindowManager *, int);
struct TextWindow *TextWindow_Find(int);
int TextWindowManager_AllocateWindowId(struct WindowManager *);
const u8 *TextWindow_ResolveString(struct TextWindow *, int);
int TextWindow_Destroy(int id)
{
    struct WindowManager *p = gWindowManager;
    struct TextWindow *w;
    struct TextWindow *previous;
    struct TextWindow **nextSlot;
    struct TextWindow **prevSlot;
    struct TextWindow **link;
    if (!p || !(w = TextWindowManager_FindWindow(p, id)))
        return -1;
    TextWindow_BlankRect(w->x, w->y, w->width, w->height);
    w->callback = TextWindow_OnIdle;
    link = (struct TextWindow **)w->next;
    nextSlot = &w->next;
    if (link) {
        link = &((struct TextWindow *)link)->previous;
        prevSlot = &w->previous;
        *link = *prevSlot;
    } else {
        prevSlot = &w->previous;
        p->head = *prevSlot;
    }
    link = prevSlot;
    previous = *link;
    if (previous)
        previous->next = *nextSlot;
    *nextSlot = NULL;
    *link = NULL;
    Heap_Free(w);
    return id;
}
