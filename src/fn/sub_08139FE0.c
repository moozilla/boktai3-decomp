#include "global.h"
#include "actor_attachment_state.h"
#include "background_controller.h"
#include "player_cue_sequence.h"

struct FollowingPosition {
    s32 x, y, z, w;
};
struct FollowingCamera {
    u8 unk_00[0x44];
    s32 x;
};
struct FollowingEffect {
    void (*update)(struct FollowingEffect *);
    u8 collider[0x5c];                /* 0x04 */
    struct ContactSwarmSprite sprite; /* 0x60 */
    struct DisplaySequenceState anim; /* 0x8c */
    void *sequence;
    struct FollowingPosition *anchor;  /* 0xa0 */
    struct FollowingPosition position; /* 0xa4 */
    u8 unk_B4[0x10];
    s32 *height; /* 0xc4 */
    u8 unk_C8[4];
    u16 duration;
    u8 active, entered;
};
s32 Div(s32, s32);
void Collider_SetFixedPositionWithYOffset(void *, struct FollowingPosition *, s32);
void SpatialIndex_InsertEntry(void *);
void FollowingSpriteEffect_SetStateCallback(struct FollowingEffect *,
                                            void (*)(struct FollowingEffect *));
void FollowingSpriteEffect_OnStop(struct FollowingEffect *);

static inline u32 FollowingSpriteEffect_StepSprite(struct ContactSwarmSprite *sprite,
                                                   struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(sprite, state);
}
/* Tentative: ease toward the anchor while playing the attached sprite. */
void FollowingSpriteEffect_UpdateFollowing(struct FollowingEffect *p)
{
    struct FollowingPosition target;
    s32 delta, height, pixels;
    struct FollowingPosition *pos;
    struct ContactPosition *dst;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->position = *p->anchor;
        p->entered = 0;
    }
    p->position.y = p->anchor->y;
    p->position.z = p->anchor->z + 0x12000;
    target.x = p->anchor->x + 0x40000;
    delta = Div(target.x - p->position.x, 8);
    pixels = delta >> 12;
    pos = &p->position;
    if (pixels > 6)
        delta = Div(target.x - pos->x, 4);
    else if (pixels > 4)
        delta = Div(target.x - pos->x, 6);
    pos->x += delta;
    height = *p->height - 0x20000;
    dst = &p->sprite.position;
    dst->x = (pos->x >> 12) - ((struct FollowingCamera *)gSceneCamera)->x;
    dst->y = ((s32)(pos->z + (pos->y + height)) >> 12) + 0x60;
    dst->z = pos->z >> 12;
    FollowingSpriteEffect_StepSprite(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, pos, height);
    SpatialIndex_InsertEntry(p->collider);
    if (!--p->duration)
        FollowingSpriteEffect_SetStateCallback(p, FollowingSpriteEffect_OnStop);
}
