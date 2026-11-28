#include "monsters/screamer/screamer_playback.h"
s32 Screamer_UpdateScream(struct ScreamerState *);
struct ContactPosition *Player_GetEffectivePosition(u32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *screamer)
{
    struct ContactPosition *player;
    struct ContactPosition *position;
    s32 dx, dz;
    screamer->sprite.flags &= ~1;
    player = Player_GetEffectivePosition(0);
    position = &screamer->sprite.position;
    dx = player->x - position->x;
    dz = player->z - position->z;
    if (dx * dx + dz * dz < (s32)screamer->f1FC) {
        Anim_RestartSequence(&screamer->sequence, screamer->f218, 0x1D, (u8)screamer->f1D8, 0);
        screamer->update = Screamer_UpdateScream;
        screamer->age = 0;
        screamer->f1D6 = 0;
    }
    screamer->bodyCollider.flags &= ~4;
    Collider_SetPosition(&screamer->bodyCollider, position, 0);
    screamer->frontCollider.flags |= 4;
    Screamer_StepAnimation(screamer);
    return 0;
}
