#include "paired_scene_actor.h"
#include "ram.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
void PairedSceneActor_UpdateHalves(void);
void sub_081B5E38(void);
void *Vanargand_CreatePartController(void *origin, u16 (*palettes)[16], s32 mode)
{
    void **global = &gVanargandPartController;
    struct VanargandAttackPair *p = Task_Create(9, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, PairedSceneActor_UpdateHalves, sub_081B5E38);
        *global = p;
        if (sub_081B5EA4(p, origin, palettes, mode) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
