#include "scene_particle_pool.h"
#define SceneParticlePool_LoadResources sub_08068840
void SceneParticlePool_LoadResources(struct SceneParticlePool *pool)
{
    pool->graphicsResource = AnimData_FindEntry(0x1C1A);
    pool->sequenceResource = ScriptProc_Start(0x922E, 0xD1B8);
}
