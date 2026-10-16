#include "global.h"
#include "engine.h"
extern void *gUnk_020005AC; /* the Hresvelgr task */
void *sub_0802140C(u32);
void sub_081A2BD4(void *);
void sub_081A2C3C(void *);
s32 sub_081A2C80(void *, void *);
/* Registry routine C077: returns the singleton in slot -t (default 1), or
   creates the Hresvelgr task and records it in gUnk_020005AC. */
void *Hresvelgr_Create(void *arg)
{
    void *p = sub_0802140C(Script_GetKeywordValue('t', 1));
    if (p)
        return p;
    p = Task_Create(9, 0x7E8);
    if (p) {
        Task_SetCallbacks(p, sub_081A2BD4, sub_081A2C3C);
        if (sub_081A2C80(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gUnk_020005AC = p;
    return p;
}
