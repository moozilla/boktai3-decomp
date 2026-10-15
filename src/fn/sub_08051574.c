#include "directional_force.h"
#define DirectionalForce_CreateFromScript sub_08051574
s32 sub_080513B8(struct DirectionalForce *, u32);
s32 sub_08051224(struct DirectionalForce *);
void sub_08051390(void);
struct DirectionalForce *DirectionalForce_CreateFromScript(u32 id)
{
    struct DirectionalForce *force;
    if (gUnk_02000114)
        return gUnk_02000114;
    force = Task_Create(9, 0x30);
    if (force) {
        Task_SetCallbacks(force, (void (*)(void *))sub_08051224, (void (*)(void *))sub_08051390);
        if (sub_080513B8(force, id) < 0) {
            Task_Kill(force);
            return 0;
        }
    }
    return force;
}
