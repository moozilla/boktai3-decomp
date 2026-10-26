#include "directional_force.h"
s32 DirectionalForce_InitializeFromScript(struct DirectionalForce *, u32);
s32 DirectionalForce_Tick(struct DirectionalForce *);
void DirectionalForce_Destroy(void);
struct DirectionalForce *DirectionalForce_CreateFromScript(u32 id)
{
    struct DirectionalForce *force;
    if (gDirectionalForce)
        return gDirectionalForce;
    force = Task_Create(9, 0x30);
    if (force) {
        Task_SetCallbacks(force, (void (*)(void *))DirectionalForce_Tick,
                          (void (*)(void *))DirectionalForce_Destroy);
        if (DirectionalForce_InitializeFromScript(force, id) < 0) {
            Task_Kill(force);
            return 0;
        }
    }
    return force;
}
