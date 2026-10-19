#include "global.h"
#include "scene_actor_control_phase.h"
/* Tentative phase-control interpretation; names have low confidence. */
#define SceneActorPhase_UpdateInitialMotion sub_080B03BC
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
static inline void SelectControl(struct SceneActorRegistryResetView *p, void *control, u32 kind)
{
    p->field2B2 = 1;
    p->field2B0 = 0;
    p->field2B5 = kind;
    p->target = control;
    p->field2C8 = 0;
    {
        u32 *flags = &p->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    p->word104 = 0;
}
void sub_080B03BC(struct SceneActorRegistryResetView *p)
{
    if (SceneActorControl_TakeFlag(&p->field2B1)) {
        void *control = p->entryTarget224;
        u32 kind = 2;
        SelectControl(p, control, kind);
    }
    if (!(u8)sub_0824923C(p, p->update270)) {
        u8 done = sub_0812F3CC(p);
        if (!done) {
            if (p->field2B0) {
                u32 phase = 0x13;
                p->state2A8 = done;
                p->phase2A9 = done;
                p->behavior2AA = phase;
                p->counter2AC = done;
                p->field2B1 = 1;
                SelectControl(p, p->savedTarget, 0);
                p->word104 = (u32)p->entryTarget228;
                p->word108 = done;
                p->word10C = done;
                p->word110 = 1;
                p->flags114 = 1;
            } else
                ++p->counter2AC;
        }
    }
}
