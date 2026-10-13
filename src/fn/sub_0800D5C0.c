#include "falling_objects.h"
void sub_0800CC34(void *);
void sub_08015F60(u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);
s32 AnimatedParticles_EmitBurst(void *, u32, u32, u32, u32, u32, u32, u32);
s32 FallingObject_SpawnFinishParticles(struct FallingObject *object)
{
    struct FallingPackedVec position;
    position.x = object->sprite.position.x;
    position.y = object->sprite.position.y;
    position.z = object->sprite.position.z;
    return AnimatedParticles_EmitBurst(&position, 2, 0, 64, 8, 4, 0, 0);
}
