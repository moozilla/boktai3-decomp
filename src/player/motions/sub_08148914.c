#include "player/cues/player_cue.h"
#include "player/player.h"
#include "player/cutscene/player_cue_sequence.h"
#include "bosses/vanargand/vanargand_projectiles.h"
u32 Player_ConsumeStateEntry(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_SetActionPhase(struct Player *, s32);
void Sound_PlaySong(u32);
extern const s16 gUnk_086149C4[];
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
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.setupResource, 1, p->heading,
                     p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
    if (p->directionFrame) {
        u32 angle = ((p->facing + 5) & 7) << 5;
        p->velocityX =
            -VanargandProjectile_Scale(gUnk_086149C4[(angle + 64) & 255] * p->directionFrame);
        p->velocityZ = -VanargandProjectile_Scale(gUnk_086149C4[angle] * p->directionFrame);
        p->directionFrame = (p->directionFrame * 3) >> 2;
    } else {
        p->contactSpecial = 0;
        if (p->movementModifierKind == 3 && p->movementModifierActive)
            ActorSequence_Begin(p, 22, 0);
        else
            ActorSequence_Begin(p, 0, 0);
    }
}
