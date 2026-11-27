#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_state.h"
#include "ui/dialogue/text_window_manager.h"
#include "ui/dialogue/text_window.h"
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
static inline void TextWindow_Link(struct WindowManager *p, struct TextWindow *w)
{
    if (p->head)
        p->head->next = w;
    w->next = 0;
    w->previous = p->head;
    p->head = w;
}
int TextWindow_Create(int x, int y, int width, int height)
{
    struct WindowManager *p = gWindowManager;
    struct TextWindow *w;
    struct TextState *text;
    if (!p)
        return -1;
    w = Heap_Alloc(sizeof(struct TextWindow));
    if (!w)
        return -1;
    Memory_Clear(w, sizeof(struct TextWindow));
    w->id = TextWindowManager_AllocateWindowId(p);
    if (w->id < 0) {
        Heap_Free(w);
        return -1;
    }
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->enabled = 1;
    w->clock = 0;
    w->index = 0;
    w->symbol = -1;
    w->script = 0;
    w->sequenceIndex = 0;
    w->sequenceCount = 1;
    w->next = 0;
    w->previous = 0;
    w->callback = TextWindow_OnIdle;
    text = &w->text;
    TextState_Init(text, w->x, w->y, w->width, w->height);
    text->fresh = 0;
    text->busy = 1;
    TextWindow_Link(p, w);
    return w->id;
}
