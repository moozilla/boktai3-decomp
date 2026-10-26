#include "global.h"
#include "engine.h"
#include "ram.h"
void *SceneActors_GetBySlot(u32);
void ActorReaction_Tick(void *);
void Hresvelgr_Destroy(void *);
s32 sub_081A2C80(void *, void *);
/* Registry routine C077: returns the singleton in slot -t (default 1), or
   creates the Hresvelgr task and records it in gHresvelgr. */
void *Hresvelgr_Create(void *arg)
{
    void *p = SceneActors_GetBySlot(Script_GetKeywordValue('t', 1));
    if (p)
        return p;
    p = Task_Create(9, 0x7E8);
    if (p) {
        Task_SetCallbacks(p, ActorReaction_Tick, Hresvelgr_Destroy);
        if (sub_081A2C80(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gHresvelgr = p;
    return p;
}
