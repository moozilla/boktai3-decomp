#include "monsters/popbud/popbud_playback.h"
/* Names and field interpretations are tentative. */
struct ContactPosition *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 ProjectileEmitter_UpdateAttack(struct PinkSpineState *);
u32 ProjectileEmitter_WaitForPlayerApproach(struct PinkSpineState *);
u32 ProjectileEmitter_UpdateAttack(struct PinkSpineState *p)
{
    struct ContactPosition *pos = Player_GetEffectivePosition(0);
    struct ContactPosition *origin = &p->sprite.position;
    s32 x = pos->x - origin->x;
    s32 z = pos->z - origin->z;
    s32 xx = x * x, zz = z * z;
    s32 distance = xx + zz;
    s32 remaining, span;
    if (distance < p->farRadiusSquared)
        p->f130 = 1;
    remaining = p->nearRadiusSquared - distance;
    span = p->nearRadiusSquared - p->farRadiusSquared;
    if (remaining > (span * 9) / 16)
        Anim_SetDurationScale(&p->sequence, 0x20);
    else if (remaining < (span * 3) / 4)
        Anim_SetDurationScale(&p->sequence, 0x80);
    else
        Anim_SetDurationScale(&p->sequence, 0x40);
    if (distance > p->nearRadiusSquared) {
        Anim_RestartSequence(&p->sequence, p->f15C, 11, 0, 0);
        p->update = ProjectileEmitter_WaitForPlayerApproach;
    }
    return 0;
}
