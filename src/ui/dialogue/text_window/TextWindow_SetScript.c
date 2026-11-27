#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_state.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
void Heap_Free(void *);
void TextWindow_BlankRect(int, int, int, int);
void TextState_Init(struct TextState *, int, int, int, int);
void TextState_SetBounds(struct TextState *, int, int, int, int);
void TextState_Reset(struct TextState *);
int Script_ParseStringRef(const void *);
const u8 *Text_LookupString(int);
void TextWindow_OnIdle(void);
void TextWindow_OnPlayback(void);
struct TextWindow *TextWindowManager_FindWindow(struct WindowManager *, int);
struct TextWindow *TextWindow_Find(int);
int TextWindowManager_AllocateWindowId(struct WindowManager *);
const u8 *TextWindow_ResolveString(struct TextWindow *, int);
int TextWindow_SetScript(int id, const u8 *script)
{
    struct TextWindow *w = TextWindow_Find(id);
    struct TextState *t;
    int result;
    if (!w)
        goto fail;
    t = &w->text;
    w->script = script;
    t->pc = TextWindow_ResolveString(w, w->index);
    w->index = 0;
    w->sequenceIndex = 0;
    w->sequenceCount = 1;
    w->symbol = 0;
    TextState_Reset(t);
    result = 0;
    goto done;
fail:
    result = -1;
done:
    return result;
}
