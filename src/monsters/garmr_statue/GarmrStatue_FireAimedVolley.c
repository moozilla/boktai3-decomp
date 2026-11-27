#include "engine/math/math_shift.h"
#include "monsters/garmr_statue.h"
#include "engine/math/scene_vec.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine_globals.h"
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
struct SceneVec *Player_GetEffectivePosition(s32);
void Sound_PlaySong(u32);
void sub_0820843C(void);
void SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                   s32, s32, u32, s32, u32, u32);
static inline void CopyPosition(struct SceneVec *out, struct SceneVec *in)
{
    *out = *in;
}
static inline void Polar(struct SceneVec *v, s32 angle, s32 length)
{
    v->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * length, 12);
    v->y = 0;
    v->z = Math_ShiftTowardZero(gUnk_086149C4[angle] * length, 12);
}
u32 GarmrStatue_FireAimedVolley(struct GarmrStatue *p)
{
    struct SceneVec position, velocity;
    struct SceneVec *pos, *player, *origin;
    u8 angle;
    s32 half, speed, range;
    const u16 *sampleOrigin;
    u32 effectMask = 0x400000;
    u32 effectKind = 0xb6;
    if (Mod(p->fireTimer, 10) == 0 && p->fireTimer > 45) {
        pos = &position;
        origin = &p->sprite.coordinates.position;
        CopyPosition(pos, origin);
        pos->y += 128;
        player = Player_GetEffectivePosition(0);
        angle = Math_Atan2(player->x - origin->x, player->z - origin->z);
        {
            PARTICLE_RANDOM_SAVE_ORIGIN(sampleOrigin, gRandomTable);
            angle = angle + (ParticleRandom_ReadDirectSample(sampleOrigin) & 31) - 16;
            half = p->projectileSpeed >> 1;
            range = p->projectileSpeed - half;
            speed = half;
            if (range != 0)
                speed += Mod(ParticleRandom_ReadDirectSample(gRandomTable), range);
        }
        Polar(&velocity, angle, speed);
        if (Mod(p->fireTimer, 20) == 0) {
            SpriteEffectController_Create(&position, &velocity, 0x4384, 0xf62b, 0, 0x200, 9, effectKind,
                                          2, 0, 0, 0, p->effectA, p->effectB, effectMask,
                                          p->effectC, 0, 0);
        } else {
            SpriteEffectController_Create(&position, &velocity, 0x4384, 0xf62b, 0, 0x400, 9, effectKind,
                                          2, 0, 0, 0, p->effectA, p->effectB, effectMask,
                                          p->effectC, 0, 0);
        }
        Sound_PlaySong(0x51b);
    }
    if (p->fireTimer > 0)
        p->fireTimer--;
    else
        p->update = sub_0820843C;
    return 0;
}
