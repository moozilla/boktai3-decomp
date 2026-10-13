#include "choice_menu.h"
#define ChoiceMenu_Create sub_08006CD0
void *sub_08219FBC(s32, s32);
void sub_0821A0C0(void *);
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
