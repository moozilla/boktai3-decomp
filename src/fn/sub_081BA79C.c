#include "global.h"
#include "input.h"

struct Registry3F74Resource {
    u8 unknown[0x1A4];
    u32 flags;
};
struct Registry3F74Object {
    u8 unknown[0x228];
    s32 selection;
    struct Registry3F74Resource *resource;
    u32 selectionScripts[11];
    u32 alternateScript;
};
void MenuCursor_SetSelectionFlag(void *);
void Registry3F74_StartClosingFade(void *);
void Script_Call(u32, void *);
static inline u32 TestFlags(struct Registry3F74Resource *p, u32 mask)
{
    return p->flags & mask;
}
void Registry3F74_ConfirmSelection(struct Registry3F74Object *p)
{
    if (gPadState[1] & 1) {
        if (TestFlags(p->resource, 2) && p->selection >= 0) {
            if (p->selection == 1 && p->alternateScript) {
                Script_Call(p->alternateScript, 0);
            } else if (p->selectionScripts[p->selection]) {
                Registry3F74_StartClosingFade(p);
                MenuCursor_SetSelectionFlag(p->resource);
            }
        }
    }
}
