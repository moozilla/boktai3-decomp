#include "contact_actor.h"
#include "pink_spine_playback.h"
#include "engine.h"
#include "monster_particle_random.h"

void PinkSpine_RecordDefeat(struct PinkSpineState *);
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);
u32 ProjectileEmitter_EmitAttackFrame(struct PinkSpineState *p)
{
    struct ContactPosition position;
    struct ContactPosition offset;
    if (!p->f13A) {
        s32 frame = (s32)p->age;
        if (frame <= 8) {
            PinkSpine_ProjectileOrigin(&position, p);
            {
                u32 sample = (u32)gRandomTable;
                sample = ParticleRandom_ReadDirectSample((const u16 *)sample);
                offset.x = (sample & 15) - 8;
                offset.y = 0;
                offset.z = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) - 8;
            }
            if (frame & 1)
                SpriteEffectController_Create(&position, &offset, 0x61F9, 0x31F4, 8, 0x200, 9,
                                              0x1C6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            else
                SpriteEffectController_Create(&position, &offset, 0x61F9, 0x31F4, 8, 0x400, 9,
                                              0x1C6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            if ((s32)p->age <= 8)
                Combat_SpawnDirectionalProjectile(p, p->age - 1);
        }
        if (p->age == 9)
            PinkSpine_RecordDefeat(p);
    }
    return 0;
}
