#include "monsters/poison_hand.h"
struct ContactPosition *Player_GetEffectivePosition(u32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void PoisonHand_UpdateEmergence(void);
u32 PoisonHand_WaitForPlayerApproach(struct PoisonHand *p)
{
    struct ContactPosition *pos = Player_GetEffectivePosition(0);
    struct ContactPosition *origin = &p->sprite.position;
    s32 x = pos->x - origin->x;
    s32 z = pos->z - origin->z;
    s32 xx = x * x, zz = z * z;
    if (xx + zz < p->attackDistanceSquared) {
        p->update = PoisonHand_UpdateEmergence;
        Anim_RestartSequence(&p->sequence, p->animationResource, 24, 0, 8);
        p->countdown = 180;
        Sound_PlaySong(0x51c);
    }
    return 0;
}
