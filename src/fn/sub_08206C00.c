// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"

struct VulcanCannon {
    u8 pad0[0x18];
    s32 timer;
    u8 pad1c[0x6c - 0x1c];
    union {
        struct DisplaySequenceSprite sprite;
        struct {
            u8 pad[0x1c];
            struct SceneVec position;
        } spatial;
    } display;
    u8 pad98[0x108 - 0x98];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s32 range;
    u8 pad120[0x132 - 0x120];
    s8 facing;
    u8 unknown133;
    s16 turnDelay, unknown136, shotCount, targetVisible;
    u8 pad13c[6];
    s16 aimTimer;
    u8 pad144[0x170 - 0x144];
    u32 useDelayTable;
    u8 pad174[8];
    void *resource;
    u8 pad180[0x194 - 0x180];
    s16 delayCount, delayIndex, delays[1];
};
struct SceneVec *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 VulcanCannon_HasClearShot(struct VulcanCannon *);
s32 Math_Atan2(s32, s32);
void VulcanCannon_UpdateFacingTurn(void);
void VulcanCannon_UpdateFiringBurst(void);
static inline u32 VulcanCannon_StepIdleAnimation(struct DisplaySequenceSprite *sprite,
                                                 struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(sprite, state);
}

static inline s32 TargetDistance(struct SceneVec *player, struct SceneVec *position)
{
    struct SceneVec *a = player;
    struct SceneVec *b = position;
    s32 dx, dz;
    dx = a->x - b->x;
    dz = a->z - b->z;
    return dx * dx + dz * dz;
}
static inline s32 TargetAngle(struct SceneVec *player, struct SceneVec *position)
{
    register struct SceneVec *a asm("r2") = player;
    register struct SceneVec *b asm("r3") = position;
    return Math_Atan2(a->x - b->x, a->z - b->z);
}

s32 VulcanCannon_UpdateTargeting(struct VulcanCannon *p)
{
    register struct SceneVec *player asm("r0") = Player_GetEffectivePosition(0);
    s32 distance = TargetDistance(player, &p->display.spatial.position);
    struct SceneVec target = *Player_GetEffectivePosition(0);
    asm("" : "+m"(target));
    if (p->useDelayTable) {
        if (p->timer > p->delays[p->delayIndex]) {
            if (!p->facing)
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
            else
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
            p->state = VulcanCannon_UpdateFacingTurn;
            p->timer = 0;
            p->delayIndex++;
            if (p->delayIndex > p->delayCount - 1)
                p->delayIndex = 0;
        }
    } else {
        if (p->timer > p->turnDelay && p->turnDelay != 0) {
            if (!p->facing)
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
            else
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
            p->timer = 0;
            p->state = VulcanCannon_UpdateFacingTurn;
        }
    }
    if (distance < p->range * p->range) {
        if (!p->facing) {
            if (target.z > p->display.spatial.position.z - 0x80 &&
                target.z < p->display.spatial.position.z + 0x80 &&
                target.x > p->display.spatial.position.x) {
                if (VulcanCannon_HasClearShot(p)) {
                    p->aimTimer = 0;
                    p->targetVisible = 1;
                    p->timer = 0;
                    p->shotCount = 0;
                    p->state = VulcanCannon_UpdateFiringBurst;
                } else
                    p->targetVisible = 0;
            } else
                p->targetVisible = 0;
        } else {
            if (target.x > p->display.spatial.position.x - 0x80 &&
                target.x < p->display.spatial.position.x + 0x80 &&
                target.z > p->display.spatial.position.z) {
                if (VulcanCannon_HasClearShot(p)) {
                    p->aimTimer = 0;
                    p->targetVisible = 1;
                    p->timer = 0;
                    p->shotCount = 0;
                    p->state = VulcanCannon_UpdateFiringBurst;
                } else
                    p->targetVisible = 0;
            } else
                p->targetVisible = 0;
        }
    } else
        p->targetVisible = 0;
    if (p->aimTimer == 119) {
        u8 angle;
        register s32 facing asm("r5");
        player = Player_GetEffectivePosition(0);
        angle = TargetAngle(player, &p->display.spatial.position);
        p->timer = 0;
        p->shotCount = 0;
        facing = p->facing;
        if (!facing) {
            if ((u8)(angle - 0x21) <= 0x7e) {
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
                p->timer = facing;
                p->state = VulcanCannon_UpdateFacingTurn;
            }
        } else {
            if ((u8)(angle - 0x21) > 0x7e) {
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
                p->timer = 0;
                p->state = VulcanCannon_UpdateFacingTurn;
            }
        }
    }
    VulcanCannon_StepIdleAnimation(&p->display.sprite, &p->sequence);
    return 0;
}
