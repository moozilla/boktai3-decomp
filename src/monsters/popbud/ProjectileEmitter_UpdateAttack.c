#include "monsters/popbud/popbud_playback.h"
/* Names and field interpretations are tentative. */
struct ContactPosition *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 ProjectileEmitter_UpdateAttack(struct PinkSpineState *);
u32 ProjectileEmitter_WaitForPlayerApproach(struct PinkSpineState *);
u32 ProjectileEmitter_UpdateAttack(struct PinkSpineState *popbud)
{
    struct ContactPosition *pos = Player_GetEffectivePosition(0);
    struct ContactPosition *origin = &popbud->sprite.position;
    s32 x = pos->x - origin->x;
    s32 z = pos->z - origin->z;
    s32 distance = x * x + z * z;
    s32 remaining, span;
    if (distance < popbud->farRadiusSquared)
        popbud->f130 = 1;
    remaining = popbud->nearRadiusSquared - distance;
    span = popbud->nearRadiusSquared - popbud->farRadiusSquared;
    if (remaining > (span * 9) / 16)
        Anim_SetDurationScale(&popbud->sequence, 0x20);
    else if (remaining < (span * 3) / 4)
        Anim_SetDurationScale(&popbud->sequence, 0x80);
    else
        Anim_SetDurationScale(&popbud->sequence, 0x40);
    if (distance > popbud->nearRadiusSquared) {
        Anim_RestartSequence(&popbud->sequence, popbud->f15C, 11, 0, 0);
        popbud->update = ProjectileEmitter_WaitForPlayerApproach;
    }
    return 0;
}
