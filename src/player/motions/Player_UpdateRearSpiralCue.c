#include "scene/collision/contact_swarm.h"
#include "player/player.h"
#include "player/cutscene/player_cue_sequence.h"
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
// confidence=low: emits a rear-offset cue when the spiral action begins.
extern const s16 gUnk_086149C4[];
static inline s32 Scale12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void Player_PlayPositionEffect(struct Player *, struct SceneVec *, s16, u32, struct SceneVec *, s32,
                               u32);
void Player_UpdateRearSpiralCue(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        struct SceneVec pos = p->position;
        s32 angle = ((p->facing + 5) & 7) << 5;
        pos.x += Scale12(gUnk_086149C4[(angle + 64) & 255] * 64);
        pos.y += 64;
        pos.z += Scale12(gUnk_086149C4[angle] * 64);
        Player_PlayPositionEffect(p, &p->position, 1, 1, &pos, 0, 0x3A1);
    }
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 0, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
    if (++p->tetherFrame > 15)
        ActorSequence_Begin(p, 0, 0);
}
