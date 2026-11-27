#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_state.h"
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
