#include "monsters/popbud/popbud_playback.h"
/* Names and field interpretations are tentative. */
struct ContactPosition *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
u32 ProjectileEmitter_UpdateAttack(struct PinkSpineState *);
u32 ProjectileEmitter_WaitForPlayerApproach(struct PinkSpineState *popbud)
{
    struct ContactPosition *pos = Player_GetEffectivePosition(0);
    struct ContactPosition *origin = &popbud->sprite.position;
    s32 x = pos->x - origin->x;
    s32 z = pos->z - origin->z;
    if (x * x + z * z < popbud->nearRadiusSquared) {
        Anim_RestartSequence(&popbud->sequence, popbud->f15C, 12, 0, 0);
        popbud->update = ProjectileEmitter_UpdateAttack;
        popbud->f12A = 180;
        Sound_PlaySong(0x521);
    }
    return 0;
}
