#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
/* Vanargand_UpdateDescendTrack: starts the unmirrored animation, then runs two motion-track legs (origin heights 0x300 then 0x100) and on completion resets the part animation and flags the state finished (provisional name) */
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
void Anim_SetFrameDurationScale(void *, u32);
static inline u8 complete(struct VanargandPairAttack *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}

static inline void initialize(struct MotionTrack *track, const void *data, s16 x, s16 y, s16 z,
                              const struct MotionTrackVector *target, u32 mode,
                              struct MotionTrackVector *origin)
{
    s16 *q = &origin->x;
    *q = x;
    origin->y = y;
    origin->z = z;
    MotionTrack_Initialize(track, data, origin, target, mode);
}

void Vanargand_UpdateDescendTrack(struct VanargandPairAttack *p)
{
    void *animation = &p->animation;
    u32 mask = 4;
    struct MotionTrackVector first, second;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    if (p->pending) {
        p->pending = 0;
        p->phase = 0;
        p->timer = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 2, 1, mask);
        Anim_SetFrameDurationScale(animation, 0x20);
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                initialize(&p->track, &p->position, 0x400, 0x300, 0x400, 0, mask, &first);
                p->timer++;
            }
            MotionTrack_Update(&p->track);
            if (complete(p)) {
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1:
            if (p->timer == 0) {
                initialize(&p->track, &p->position, 0x400, 0x100, 0x400, 0, 2, &second);
                p->timer++;
            }
            MotionTrack_Update(&p->track);
            if (complete(p)) {
                struct VanargandDamagePart *part = &p->part;
                part->sprite.frame = 7;
                p->part.animation = 0;
                p->mode = 1;
                p->finished = 1;
                p->phase = 2;
                p->timer = 0;
            }
            break;
        case 2:
        default:
            break;
    }
}
