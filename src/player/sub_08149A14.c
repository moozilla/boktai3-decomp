#include "engine/shared/contact_swarm.h"
#include "engine/shared/player.h"
#include "player/player_cue/player_cue_sequence.h"
#include "bosses/vanargand_projectile/vanargand_projectiles.h"
// confidence=low: provisional action interpretation from retained C and assembly.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
void RisingSprite_Spawn(void *);
void SparkParticle_Spawn(void *, s32, s32, s32, s32, s32, s32, s32);
void Camera_RequestShake(u32);
extern const s16 gUnk_086149C4[];
#define B(o)  (*(u8 *)((u8 *)p + (o)))
#define H(o)  (*(u16 *)((u8 *)p + (o)))
#define SH(o) (*(s16 *)((u8 *)p + (o)))
static inline void Approach(struct Player *p)
{
    s32 x = p->terrainTarget->x - p->position.x;
    s32 z = p->terrainTarget->z - p->position.z;
    if (x >= 0)
        SH(0x38) = x >> 3;
    else
        SH(0x38) = -(-x >> 3);
    if (z >= 0)
        SH(0x3C) = z >> 3;
    else
        SH(0x3C) = -(-z >> 3);
}
static inline void Approach2(struct Player *p)
{
    s16 *target = *(s16 **)((u8 *)p + 0x47C);
    s32 x, z;
    x = target[0] - SH(0x30);
    z = target[2] - SH(0x34);
    if (x >= 0)
        SH(0x38) = x >> 3;
    else
        SH(0x38) = -(-x >> 3);
    if (z >= 0)
        SH(0x3C) = z >> 3;
    else
        SH(0x3C) = -(-z >> 3);
}
// confidence=low: moves toward a saved effect position or recoils while shake decays.
void Player_UpdateRecoilMotion(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (H(0x4C2)) {
            Sound_PlaySong(H(0x4C2));
            H(0x4C2) = 0;
        }
    }
    Anim_SetSequence(&p->cueCollider.sequence, *(void **)((u8 *)p + 0x410), 2, p->heading, p->side);
    PlayerCue_StepSequence((void *)((u8 *)p + 0x14C), &p->cueCollider.sequence);
    {
        if (p->directionFrame) {
            if (p->terrainClampBlocked) {
                Approach(p);
                if (H(0x45A) <= 5) {
                    if (!(H(0x45A) & 1)) {
                        Sound_PlaySong(0x191);
                        RisingSprite_Spawn(&p->position);
                    }
                    H(0x45A)++;
                }
            } else if (B(0x47B)) {
                Approach2(p);
                if (H(0x45A) <= 5) {
                    if (!(H(0x45A) & 1)) {
                        Sound_PlaySong(0x191);
                        SparkParticle_Spawn(&p->position, 1, 0, 0, 0, 12, 2, 2);
                    }
                    H(0x45A)++;
                }
            } else if (B(0x489)) {
                Approach2(p);
                H(0x45A)++;
            } else {
                s32 angle = ((p->facing + 5) & 7) << 5;
                SH(0x38) = -VanargandProjectile_Scale(gUnk_086149C4[(angle + 64) & 255] * H(0x270));
                SH(0x3C) = -VanargandProjectile_Scale(gUnk_086149C4[angle] * H(0x270));
            }
            Camera_RequestShake(H(0x270));
            H(0x270) = H(0x270) * 3 >> 2;
        } else {
            H(0x270) = 0;
            B(0x479) = 0;
            B(0x47B) = 0;
            B(0x489) = 0;
            B(0x4BA) = 0;
            if (B(0x5B9) == 3 && B(0x5BB))
                ActorSequence_Begin(p, 22, 0);
            else {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
        }
    }
}
