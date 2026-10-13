#include "choice_menu.h"
struct Args {
    u32 count : 16;
    u32 unused : 16;
    u32 *values;
};
void sub_0821AD08(u32, struct Args *);
void sub_0824923C(s32, void *);
static inline u32 selectionScript(struct ChoiceMenu *p)
{
    return p->script;
}
void ChoiceMenu_DispatchSelection(struct ChoiceMenu *p, s32 selection)
{
    u32 values[5];
    struct Args args;
    if (p->useScript) {
        s32 i;
        u32 script;
        values[0] = selection;
        i = 0;
        for (; i < 4; i++)
            values[i + 1] = p->arguments[i];
        args.count = 5;
        args.values = values;
        script = selectionScript(p);
        if (script)
            sub_0821AD08(script, &args);
    } else
        sub_0824923C(selection, p->callback);
}
