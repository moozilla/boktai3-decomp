#include "global.h"
void Trigger_ConsumePending(void *, u32);
struct S {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk(struct S *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_ResumeAfterAction(struct S *p)
{
    if (chk(p)) {
        u32 k = 0x10;
        p->aa = k;
    }
    if (p->af) {
        u32 two = 2;
        u32 z = 0;
        u32 one = 1;
        p->ac = two;
        p->ad = z;
        p->ae = one;
        p->c4 = z;
        p->b0 = one;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    } else
        p->c4++;
}
