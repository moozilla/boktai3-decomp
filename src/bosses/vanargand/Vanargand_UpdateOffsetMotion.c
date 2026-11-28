#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "scene/actor/motion_track.h"

void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
static inline u8 MotionComplete(struct VanargandPairAttack *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}

void Vanargand_UpdateOffsetMotion(struct VanargandPairAttack *p)
{
    void *animation = &p->animation;
    Vanargand_SetPendingCue(p, 1);
    if (p->pending) {
        struct MotionTrackVector origin;
        s16 *first;
        struct MotionTrack *track;
        s32 x, y, z;
        p->pending = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 4, 3, 4);
        x = p->position.x;
        y = p->position.y;
        z = p->position.z;
        track = &p->track;
        x -= 0x100;
        z -= 0x100;
        first = &origin.x;
        first[0] = x;
        origin.y = y;
        origin.z = z;
        MotionTrack_Initialize(track, &p->position, &origin, 0, 4);
        Sound_PlaySong(0x586);
        p->delay = 8;
    }
    MotionTrack_Update(&p->track);
    if (MotionComplete(p))
        p->finished = 1;
    else
        VanargandPairAttack_SetFlags(p, 5);
}
