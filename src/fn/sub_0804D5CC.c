#include "tether_actor.h"
extern struct TetherActor *gUnk_02000488;
s32 sub_0804D500(struct TetherActor *, u32, u32);
s32 sub_0804D378(struct TetherActor *);
struct TetherActor *sub_0804D4B0(struct TetherActor *);
#define TetherActor_CreateFromScript sub_0804D5CC
struct TetherActor *TetherActor_CreateFromScript(u32 a, u32 b)
{
    struct TetherActor *r;
    if (gUnk_02000488 != 0)
        return gUnk_02000488;
    r = Task_Create(9, 0x484);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))sub_0804D378, (void (*)(void *))sub_0804D4B0);
        if (sub_0804D500(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
