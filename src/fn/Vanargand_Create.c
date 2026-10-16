#include "global.h"
#include "engine.h"
extern void *gUnk_02000250; /* the Vanargand boss task */
void *sub_0802140C(u32);
void sub_081ADD0C(void *);
void sub_081ADD9C(void *);
s32 sub_081AE0C0(void *, void *);
/* Registry routine 7C56: creates the Vanargand task once; -t (default 5) is
   passed to sub_0802140C, which can supply an existing one. */
void *Vanargand_Create(void *arg)
{
    void *p;
    if (gUnk_02000250)
        return gUnk_02000250;
    p = sub_0802140C(Script_GetKeywordValue('t', 5));
    if (p)
        return p;
    p = Task_Create(9, 0xf48);
    if (p) {
        Task_SetCallbacks(p, sub_081ADD0C, sub_081ADD9C);
        if (sub_081AE0C0(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gUnk_02000250 = p;
    return p;
}
