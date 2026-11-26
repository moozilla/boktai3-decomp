#include "objects/warp_circle/warp_circle.h"
struct WarpCircle *Task_Create(u32, u32);
void Task_SetCallbacks(struct WarpCircle *, s32 (*)(struct WarpCircle *),
                       s32 (*)(struct WarpCircle *));
s32 WarpCircle_Initialize(struct WarpCircle *, s32, s32);
void Task_Kill(struct WarpCircle *);
s32 WarpCircle_Tick(struct WarpCircle *);
s32 WarpCircle_Destroy(struct WarpCircle *);
struct WarpCircle *WarpCircle_CreateFromScript(u32 a, u32 b)
{
    struct WarpCircle *r = Task_Create(9, 0x388);
    if (r) {
        Task_SetCallbacks(r, WarpCircle_Tick, WarpCircle_Destroy);
        if (WarpCircle_Initialize(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
