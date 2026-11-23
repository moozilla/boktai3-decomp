#include "global.h"
#include "ratatosk.h"

void Trigger_ConsumePending(void *, u32);

void SceneActor_UpdateState24AndConditionalReturnCommand(struct Ratatosk *p)
{
    u8 *q = (u8 *)p;
    if (Ratatosk_ConsumeActionEntry(p)) {
        u32 v = 0x18;
        p->actionAnimation = v;
    }
    if (p->actionFinished) {
        if (*(s16 *)(q + 0xE94) == 0) {
            u32 w = 1;
            p->actionAnimation = w;
        }
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    } else
        p->actionTimer++;
}
