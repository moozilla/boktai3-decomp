#include "global.h"
#include "engine/engine.h"

#include "gba/macro.h"
#include "gba/syscall.h"
#include "scene/collision/packed_contact_vector.h"
#include "objects/bouncing_bombs/bouncing_object.h"

struct BouncingObject *BouncingBomb_Spawn(struct PackedContactVector *,
                                          struct PackedContactVector *,
                                          struct PackedContactVector *, s32, s32, s32, s32);
// The plain conditional expression keeps each of the six expansions short before combine,
// which keeps &pos ahead of b in global allocation (r8 and sb, as in the ROM).
static inline s32 optionalValue(void)
{
    return Script_GetPc() ? Script_GetValue() : 0;
}

static inline void BouncingBomb_ClearVector(struct PackedContactVector *vector)
{
    CpuFill32(0, vector, sizeof(*vector));
}
// confidence=low: spawns a bouncing object from script keywords p (position), a (shot,
// category), v (velocity), l and n, and sets flag 1 at 0x134.
void BouncingBomb_SpawnFromScript(void)
{
    struct PackedContactVector pos;
    struct PackedContactVector vel;
    struct PackedContactVector target;
    s32 shot;
    s32 strength;
    s32 id;
    s32 category;
    struct BouncingObject *object;
    category = 0;
    shot = 0;
    if (Script_SeekToKeyword('p')) {
        pos.x = optionalValue();
        pos.y = optionalValue();
        pos.z = optionalValue();
    }
    if (Script_SeekToKeyword('a')) {
        shot = optionalValue();
        if (Script_GetPc())
            category = Script_GetValue();
        else
            category = 0;
    }
    if (Script_SeekToKeyword('v')) {
        vel.x = optionalValue();
        vel.y = optionalValue();
        vel.z = optionalValue();
    } else {
        vel.x = 0;
        vel.y = 0;
        vel.z = 0;
    }
    strength = Script_GetKeywordValue('l', 0);
    id = Script_GetKeywordValue('n', 0);
    BouncingBomb_ClearVector(&target);
    object = BouncingBomb_Spawn(&pos, &vel, &target, shot, category, strength, id);
    if (object)
        object->flags |= 1;
}
