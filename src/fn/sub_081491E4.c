#include "contact_swarm.h"
#include "player.h"
#include "player_cue_sequence.h"
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
struct ReleaseAnimation {
    u8 pad[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad2[0x3FC - 0x14C - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};
// confidence=low: emits a rear-offset cue when the spiral action begins.
#define Player_UpdateRearSpiralCue sub_081491E4
extern const s16 gUnk_086149C4[];
struct PackedPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
static inline s32 Scale12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void Player_PlayPositionEffect(void *, void *, s32, u32, void *, u32, u32);
void Player_UpdateRearSpiralCue(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        struct PackedPosition pos = *(struct PackedPosition *)&p->position;
        s32 angle = ((p->facing + 5) & 7) << 5;
        pos.x += Scale12(gUnk_086149C4[(angle + 64) & 255] * 64);
        pos.y += 64;
        pos.z += Scale12(gUnk_086149C4[angle] * 64);
        Player_PlayPositionEffect(p, &p->position, 1, 1, &pos, 0, 0x3A1);
    }
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *(const void **)((u8 *)p + 0x410), 0,
                     p->heading, p->side);
    PlayerCue_StepSequence(&((struct ReleaseAnimation *)p)->sprite,
                           &((struct ReleaseAnimation *)p)->seq);
    if (++p->tetherFrame > 15)
        ActorSequence_Begin(p, 0, 0);
}
