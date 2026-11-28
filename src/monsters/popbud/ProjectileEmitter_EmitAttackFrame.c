#include "scene/collision/contact_actor.h"
#include "monsters/popbud/popbud_playback.h"
#include "engine/engine.h"
#include "engine/math/monster_particle_random.h"

void Popbud_RecordDefeat(struct PinkSpineState *);
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);
u32 ProjectileEmitter_EmitAttackFrame(struct PinkSpineState *popbud)
{
    struct ContactPosition position;
    struct ContactPosition offset;
    if (!popbud->f13A) {
        if ((s32)popbud->age <= 8) {
            Popbud_ProjectileOrigin(&position, popbud);
            {
                u32 sample = (u32)gRandomTable;
                sample = ParticleRandom_ReadDirectSample((const u16 *)sample);
                offset.x = (sample & 15) - 8;
                offset.y = 0;
                offset.z = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) - 8;
            }
            if (popbud->age & 1)
                SpriteEffectController_Create(&position, &offset, 0x61F9, 0x31F4, 8, 0x200, 9,
                                              0x1C6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            else
                SpriteEffectController_Create(&position, &offset, 0x61F9, 0x31F4, 8, 0x400, 9,
                                              0x1C6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            if ((s32)popbud->age <= 8)
                Combat_SpawnDirectionalProjectile(popbud, popbud->age - 1);
        }
        if (popbud->age == 9)
            Popbud_RecordDefeat(popbud);
    }
    return 0;
}
