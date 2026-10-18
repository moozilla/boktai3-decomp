#define ProjectileEmitter_UpdateAttack          sub_08210180
#define ProjectileEmitter_WaitForPlayerApproach sub_08210100
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
    s32 closeDistanceSquared;
    u8 pad124[6];
    u16 timer;
    u8 pad12C[4];
    u16 closeRange;
    u8 pad132[0x2A];
    void *animationResource;
};
struct EmitterPosition *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 sub_08210180(struct ProximityProjectileEmitter *);
void Anim_SetDurationScale(void *, u32);
u32 sub_08210100(struct ProximityProjectileEmitter *);
u32 sub_08210180(struct ProximityProjectileEmitter *p)
{
    struct EmitterPosition *pos = Player_GetEffectivePosition(0);
    struct EmitterPosition *origin = &p->position;
    s32 x = pos->x - origin->x;
    s32 z = pos->z - origin->z;
    s32 xx = x * x, zz = z * z;
    s32 distance = xx + zz;
    s32 remaining, span;
    if (distance < p->closeDistanceSquared)
        p->closeRange = 1;
    remaining = p->triggerDistanceSquared - distance;
    span = p->triggerDistanceSquared - p->closeDistanceSquared;
    if (remaining > (span * 9) / 16)
        Anim_SetDurationScale(p->animation, 0x20);
    else if (remaining < (span * 3) / 4)
        Anim_SetDurationScale(p->animation, 0x80);
    else
        Anim_SetDurationScale(p->animation, 0x40);
    if (distance > p->triggerDistanceSquared) {
        Anim_RestartSequence(p->animation, p->animationResource, 11, 0, 0);
        p->update = sub_08210100;
    }
    return 0;
}
