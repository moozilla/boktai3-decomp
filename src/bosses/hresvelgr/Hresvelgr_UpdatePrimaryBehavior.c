#include "global.h"
#include "bosses/hresvelgr/offset_actor_layout.h"
extern const u32 gUnk_08611DAC[];
void Callback_InvokeActor(void *, u32);
void Hresvelgr_SelectNextBehavior(void *);
void Hresvelgr_UpdatePrimaryBehavior(struct OffsetActor *p)
{
    p->ready = 0;
    Callback_InvokeActor(p, gUnk_08611DAC[p->behavior]);
    Hresvelgr_SelectNextBehavior(p);
}
