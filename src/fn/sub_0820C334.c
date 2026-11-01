#include "screamer_playback.h"
s32 Screamer_UpdateScream(struct ScreamerState *);
struct ActorPosition *Player_GetEffectivePosition(u32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *p)
{
    struct ActorPosition *player;
    struct ActorPosition *position;
    s32 dx, dz;
    p->sprite.flags &= ~1;
    player = Player_GetEffectivePosition(0);
    position = &p->sprite.position;
    dx = player->x - position->x;
    dz = player->z - position->z;
    if (dx * dx + dz * dz < (s32)p->f1FC) {
        Anim_RestartSequence(&p->sequence, p->f218, 0x1D, (u8)p->f1D8, 0);
        p->update = Screamer_UpdateScream;
        p->age = 0;
        p->f1D6 = 0;
    }
    p->colliderFlags &= ~4;
    Collider_SetPosition(p->bodyCollider, position, 0);
    p->contactFlags |= 4;
    Screamer_StepAnimation(p);
    return 0;
}
