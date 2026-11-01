#include "global.h"
#include "display_animation.h"
#include "player_cue_sequence.h"
#include "contact_actor.h"

struct SunDiscEffectPool {
    u8 unk0[0x34];
    void *sequenceResource;
};

struct SunDiscEffect {
    u32 flags;
    u8 unk4[3];
    u8 mode;
    u8 unk8[8];
    u16 frame;
    u8 unk12[0xa];
    struct ContactPosition position;
    u8 unk24[8];
    struct DisplaySequenceState sequence;
    u8 unk3C[0x58];
    struct ContactPosition colliderPosition;
    u8 collider[0x56];
    u8 fieldF2;
    u8 unkF3[5];
    u8 resetPending;
    u8 timer;
};

void SunDiscEffect_Update(struct SunDiscEffectPool *pool, struct SunDiscEffect *effect)
{
    if (effect->resetPending) {
        effect->colliderPosition = effect->position;
        effect->position.x -= 0x80;
        Anim_SetSequence(&effect->sequence, pool->sequenceResource, 1, 0, 0);
        effect->mode = 2;
        effect->resetPending = 0;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)effect, &effect->sequence);
    Collider_SetPosition(effect->collider, &effect->colliderPosition, 0);
    Collider_Refresh(effect->collider);
    if (effect->timer)
        effect->timer--;
    else
        effect->fieldF2 = 4;
}
