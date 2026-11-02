#include "vanargand_effect.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(struct VanargandEffect *, void (*)(struct VanargandEffect *),
                       void (*)(struct VanargandEffect *));
void Task_Kill(void *);

struct VanargandEffect *VanargandEffect_Create(s32 variant, s32 delay,
                                               const struct MotionTrackVector *position,
                                               const struct MotionTrackVector *target,
                                               u32 alternate)
{
    struct VanargandEffect *obj = Task_Create(8, sizeof(*obj));
    if (obj) {
        Task_SetCallbacks(obj, VanargandEffect_Update, VanargandEffect_Destroy);
        if (VanargandEffect_Initialize(obj, variant, delay, position, target, alternate) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
