#include "contact_swarm.h"
#include "player.h"
#include "player_cue_sequence.h"
struct ReleaseAnimation {
    u8 pad[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad2[0x3FC - 0x14C - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};
u32 Player_ConsumeStateEntry(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_SetActionPhase(struct Player *, s32);
void Sound_PlaySong(u32);
extern const s16 gUnk_086149C4[];
static inline s32 ScaleRecoil(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
// confidence=low: applies a fading impulse opposite the stored facing, then chooses recovery.
void Player_UpdateDecayingRecoil(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (p->contactActionTag) {
            Sound_PlaySong(p->contactActionTag);
            p->contactActionTag = 0;
        }
        Player_SetActionPhase(p, 0);
    }
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *(const void **)((u8 *)p + 0x40C), 1,
                     p->heading, p->side);
    PlayerCue_StepSequence(&((struct ReleaseAnimation *)p)->sprite,
                           &((struct ReleaseAnimation *)p)->seq);
    {
        u32 offset = 0x270;
        u16 *magnitude;
        magnitude = (u16 *)((u8 *)p + offset);
        if (*magnitude) {
            u32 facingOffset = 0x3A4;
            u8 *facing;
            u32 angle;
            facing = (u8 *)p + facingOffset;
            angle = ((*facing + 5) & 7) << 5;
            p->velocityX = -ScaleRecoil(gUnk_086149C4[(angle + 64) & 255] * *magnitude);
            p->velocityZ = -ScaleRecoil(gUnk_086149C4[angle] * p->directionFrame);
            p->directionFrame = (p->directionFrame * 3) >> 2;
        } else {
            p->contactSpecial = 0;
            if (*((u8 *)p + 0x5B9) == 3 && *((u8 *)p + 0x5BB))
                ActorSequence_Begin(p, 22, 0);
            else
                ActorSequence_Begin(p, 0, 0);
        }
    }
}
