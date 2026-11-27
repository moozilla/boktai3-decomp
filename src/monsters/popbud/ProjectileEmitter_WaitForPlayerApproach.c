#include "monsters/popbud/popbud_playback.h"
/* Names and field interpretations are tentative. */
struct ContactPosition *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 ProjectileEmitter_UpdateAttack(struct PinkSpineState *);
u32 ProjectileEmitter_WaitForPlayerApproach(struct PinkSpineState *p)
{
    struct ContactPosition *pos = Player_GetEffectivePosition(0);
    struct ContactPosition *origin = &p->sprite.position;
    s32 x = pos->x - origin->x;
    s32 z = pos->z - origin->z;
    s32 xx = x * x, zz = z * z;
    if (xx + zz < p->nearRadiusSquared) {
        Anim_RestartSequence(&p->sequence, p->f15C, 12, 0, 0);
        p->update = ProjectileEmitter_UpdateAttack;
        p->f12A = 180;
        Sound_PlaySong(0x521);
    }
    return 0;
}
