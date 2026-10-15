#include "tether_actor.h"
extern struct TetherActor *gUnk_02000488;
s32 TetherActor_InitializeFromScript(struct TetherActor *, u32, u32);
s32 TetherActor_Tick(struct TetherActor *);
struct TetherActor *TetherActor_Destroy(struct TetherActor *);
struct TetherActor *TetherActor_CreateFromScript(u32 a, u32 b)
{
    struct TetherActor *r;
    if (gUnk_02000488 != 0)
        return gUnk_02000488;
    r = Task_Create(9, 0x484);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))TetherActor_Tick,
                          (void (*)(void *))TetherActor_Destroy);
        if (TetherActor_InitializeFromScript(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
