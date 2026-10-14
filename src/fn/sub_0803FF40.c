#include "modifier_list.h"
#define ModifierMenu_CloseScript sub_0803FF40
extern u32 gUnk_030053F0;
void sub_081093AC(u32);
static inline u32 TestFlags(u32 mask)
{
    return (gUnk_030053F4 | gUnk_030053F0) & mask;
}
void ModifierMenu_CloseScript(struct ModifierMenu *menu)
{
    void **script = &menu->script;
    if (*script) {
        Script_Call(*script, 0);
        *script = 0;
    }
    if (TestFlags(0x800))
        sub_081093AC(1);
}
