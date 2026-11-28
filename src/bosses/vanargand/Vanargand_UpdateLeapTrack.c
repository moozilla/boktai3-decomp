#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
/* Vanargand_UpdateLeapTrack: plays animation 4 with sound 0x587, rides a motion track (origin 0x280/0x300/0x280) spawning afterimages, then plays animation 0xF and once both halves are ready applies them and finishes (provisional name) */
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

void Sound_PlaySong(u32);
void VanargandAfterimagePool_Spawn(void *, void *);

void Vanargand_UpdateLeapTrack(struct VanargandPairAttack *p)
{
    void *animation = &p->animation;
    struct MotionTrackVector first;
    u32 mask = 12;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    if (p->pending) {
        p->pending = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 4, 2, 4);
        Sound_PlaySong(0x587);
        p->delay = 0x1E;
        p->phase = 0;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                initialize(&p->track, &p->position, 0x280, 0x300, 0x280, 0, 4, &first);
                p->timer++;
            }
            MotionTrack_Update(&p->track);
            if (complete(p)) {
                p->phase = 1;
                p->timer = 0;
            } else
                VanargandAfterimagePool_Spawn(p->afterimages, &p->position);
            break;
        case 1:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 0xF, 2, 2);
                Anim_SetFrameDurationScale(animation, 0x80);
                p->timer++;
                {
                    struct VanargandDamagePart *part = &p->part;
                    part->sprite.frame = 7;
                }
                p->part.animation = 0;
                p->attackParameter5B8 = 0;
                p->attackParameter5BA = 0;
            }
            if (p->animationEvent) {
                PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 0x19);
                {
                    u8 next = 2;
                    p->phase = next;
                    p->timer = 0;
                }
            }
            break;
        case 2:
            p->finished = 1;
            return;
    }
    {
        u32 mask = 1;
        p->flags |= mask;
    }
}
