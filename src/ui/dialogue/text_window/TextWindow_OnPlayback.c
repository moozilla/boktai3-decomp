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
const u8 *TextWindow_ResolveString(struct TextWindow *, int);
int TextState_StepPlayback(struct TextState *);
void TextWindow_OnPlaybackComplete(void);
void TextWindow_OnPlayback(struct WindowManager *p, struct TextWindow *w)
{
    struct TextState *t = &w->text;
    u32 i = 0;
    goto test;
next:
    i++;
test:
    if (i > w->enabled)
        goto done;
    if (!t->pc)
        goto next;
    t->entered = 0;
    if (!TextState_StepPlayback(t))
        goto next;
    if (t->fresh)
        w->callback = TextWindow_OnIdle;
    else
        w->callback = TextWindow_OnPlaybackComplete;
done:;
}
