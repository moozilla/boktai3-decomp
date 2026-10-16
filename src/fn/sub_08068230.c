#include "scene_particle_burst.h"
void SceneParticleBurst_LoadResources(struct SceneParticleBurstPool *p)
{
    p->graphicsResource = AnimData_FindEntry(0x1C1A);
    p->sequenceResource = ScriptProc_Start(0x922E, 0xD1B8);
}
