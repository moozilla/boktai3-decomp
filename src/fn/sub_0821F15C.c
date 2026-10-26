// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"

struct World {
    u8 pad[0x14];
    void *navMeshes;
};
void *ScriptProc_Start(u16, u16);

// Loads the nav-mesh archive entry for `id` and installs it on the current map.
u32 sub_0821F15C(u16 id)
{
    void *meshes = ScriptProc_Start(0xF63B, id);
    register struct World *world asm("r0") = gTerrainMap;
    world->navMeshes = meshes;
    return 0;
}
