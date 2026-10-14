#include "script_selector.h"

void Script_SetPc(u32);
u8 *Script_GetPc(void);
u32 sub_0821AA88(u8 *);
u32 sub_0821AA00(u8 *);

void ScriptSelector_MoveSelection(struct ScriptSelector *p, s32 n)
{
    u32 a = 0;
    u32 b = 0;
    s32 i;
    p->index += n;
    if (p->index < 0)
        p->index = b;
    Script_SetPc(p->scriptStart);
    for (i = 0; i <= p->index; i++) {
        if (Script_GetPc() == 0) {
            p->index = i;
            break;
        }
        a = sub_0821AA88(Script_GetPc());
        b = sub_0821AA00(Script_GetPc());
    }
    p->value = a;
    p->selectedScript = b;
}
