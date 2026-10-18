#define ProjectileEmitter_WaitForPlayerApproach sub_08210100
#define ProjectileEmitter_UpdateAttack          sub_08210180
#include "global.h"
/* Names and field interpretations are tentative. */
struct EmitterPosition {
    s16 x, y, z, pad;
};
struct ProximityProjectileEmitter {
    u8 pad0[0x40];
    struct EmitterPosition position;
    u8 pad48[0xC0];
    u32 (*update)(struct ProximityProjectileEmitter *);
    u8 animation[0x10];
    s32 triggerDistanceSquared;
    u8 pad120[0xA];
    u16 timer;
    u8 pad12C[0x30];
    void *animationResource;
};
struct EmitterPosition *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 sub_08210180(struct ProximityProjectileEmitter *);
u32 sub_08210100(struct ProximityProjectileEmitter *p)
{
    struct EmitterPosition *pos = Player_GetEffectivePosition(0);
    struct EmitterPosition *origin = &p->position;
    s32 x = pos->x - origin->x;
    s32 z = pos->z - origin->z;
    s32 xx = x * x, zz = z * z;
    if (xx + zz < p->triggerDistanceSquared) {
        Anim_RestartSequence(p->animation, p->animationResource, 12, 0, 0);
        p->update = sub_08210180;
        p->timer = 180;
        Sound_PlaySong(0x521);
    }
    return 0;
}
