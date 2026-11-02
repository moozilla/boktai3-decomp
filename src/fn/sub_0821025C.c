#include "contact_actor.h"
#include "pink_spine_playback.h"
#include "engine.h"
#include "ram.h"

// MATCHING_CONCESSION: register-pinning

void Combat_SpawnDirectionalProjectile(struct PinkSpineState *, u8);
void PinkSpine_RecordDefeat(struct PinkSpineState *);
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);
static inline s32 randomValue(u32 table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline void CopyEmitterPosition(struct ContactPosition *out, const struct ContactPosition *in)
{
    *out = *in;
}
static inline void raisePosition(struct ContactPosition *p)
{
    p->y += 0x80;
}
u32 ProjectileEmitter_EmitAttackFrame(struct PinkSpineState *p)
{
    struct ContactPosition position;
    struct ContactPosition offset;
    if (!p->f13A) {
        register s32 f asm("r12") = (s32)p->age;
        if (f <= 8) {
            CopyEmitterPosition(&position, &p->sprite.position);
            raisePosition(&position);
            {
                offset.x = (randomValue((u32)gRandomTable) & 15) - 8;
                offset.y = 0;
                offset.z = (randomValue((u32)gRandomTable) & 15) - 8;
            }
            if (f & 1)
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
