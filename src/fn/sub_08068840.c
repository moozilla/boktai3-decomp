#include "scene_particle_pool.h"
void SceneParticlePool_LoadResources(struct SceneParticlePool *pool)
{
    pool->graphicsResource = AnimData_FindEntry(0x1C1A);
    pool->sequenceResource = ScriptProc_Start(0x922E, 0xD1B8);
}
