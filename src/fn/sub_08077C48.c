#include "scene_actor_placement.h"
#include "monster_particle_random.h"
struct SceneActorEmissionPhase {
    u8 unknown0[0x2B6];
    u8 direction, unknown2B7[3];
    u8 amplitude;
    s8 amplitudeStep;
    u16 timer;
};
/* ContactPosition as packed 16-bit fields (read-modify-write of whole words). */
struct EmitPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
/* The random table's address assembled byte by byte (scope README fact 48). */
union EmitTableSource {
    const u16 *table;
    u8 bytes[4];
};
s32 sub_080148CC(s32, s32, void *, u32, s32, s32, s32, s32, s32, s32, s32);
static inline const u16 *EmitReadTable(const union EmitTableSource *source)
{
    return source->table;
}
/* Three particle bursts around the actor, raised by offset->y. */
static inline void EmitBurst(struct RoomActor *actor, const struct EmitPosition *offset, s32 kind,
                             u32 angle)
{
    union EmitTableSource source = {0};
    const u16 *table;
    struct EmitPosition position;
    struct EmitPosition jitter;
    source.bytes[0] = (u8)RANDOM_TABLE_ADDR;
    source.bytes[1] = (u8)(RANDOM_TABLE_ADDR >> 8);
    source.bytes[2] = (u8)(RANDOM_TABLE_ADDR >> 16);
    source.bytes[3] = (u8)(RANDOM_TABLE_ADDR >> 24);
    table = EmitReadTable(&source);
    /* confidence=low: a jitter that is drawn but never applied. */
    jitter.y = ParticleRandom_ReadDirectSample(table);
    jitter.z = ParticleRandom_ReadDirectSample(table);
    position = *(struct EmitPosition *)&actor->body.position;
    position.y += offset->y;
    {
        s32 count = (ParticleRandom_ReadDirectSample(table) & 3) + 2;
        s32 duration = (ParticleRandom_ReadDirectSample(table) & 3) + 6;
        sub_080148CC(count, kind, &position, angle, 8, duration, 4, 10, 8, 5, 10);
    }
    {
        u32 direction = (angle + (ParticleRandom_ReadDirectSample(table) & 7)) & 255;
        s32 duration = (ParticleRandom_ReadDirectSample(table) & 7) + 6;
        sub_080148CC(3, kind, &position, direction, 8, duration, 4, 14, 4, 6, 10);
    }
    {
        u32 direction = (angle + (ParticleRandom_ReadDirectSample(table) & 15)) & 255;
        s32 count = (ParticleRandom_ReadDirectSample(table) & 3) + 1;
        s32 duration = (ParticleRandom_ReadDirectSample(table) & 7) + 4;
        sub_080148CC(count, kind, &position, direction, 8, duration, 2, 6, 3, 5, 10);
    }
}
void SceneActorPlacement_EmitTimedParticles(struct RoomActor *actor)
{
    struct SceneActorEmissionPhase *phase = (void *)actor;
    u32 angle;
    struct EmitPosition offset;
    if (!phase->timer)
        return;
    --phase->timer;
    angle = (((phase->direction + 9) & 7) << 5);
    angle += phase->amplitude;
    phase->amplitude += phase->amplitudeStep * 4;
    if (phase->amplitude > 64) {
        s32 step;
        phase->amplitude = 64;
        step = phase->amplitudeStep;
        phase->amplitudeStep = -step;
    }
    if (Mod(phase->timer, 4))
        return;
    Memory_Clear(&offset, 8);
    switch (actor->category) {
        case 2:
            offset.y = 0;
            if (!actor->paletteOffset)
                EmitBurst(actor, &offset, 9, angle);
            else
                EmitBurst(actor, &offset, 10, angle);
            break;
        case 14:
            offset.y = 40;
            EmitBurst(actor, &offset, 11, angle);
            break;
    }
}
