// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct World {
    u8 pad[0x14];
    void *navMeshes;
};
extern struct World *gUnk_030052F4;
void *ScriptProc_Start(u16, u16);

// Loads the nav-mesh archive entry for `id` and installs it on the current map.
u32 sub_0821F15C(u16 id)
{
    void *meshes = ScriptProc_Start(0xF63B, id);
    register struct World *world asm("r0") = gUnk_030052F4;
    world->navMeshes = meshes;
    return 0;
}
