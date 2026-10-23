// MATCHING_CONCESSION: register-pinning
#include "global.h"

#define Vanargand_UpdateOffsetMotion sub_081AB438
struct MotionTrackVector {
    s16 x, y, z, padding;
};
struct MotionTrack;
struct Vanargand {
    u8 pad0[0x54];
    s16 x, y, z;
    u8 pad5A[0x9c - 0x5a];
    u32 flags;
    u8 padA0[0xaf - 0xa0];
    u8 finished;
    u8 padB0;
    u8 pending;
    u8 padB2[0x18c - 0xb2];
    u8 animation[0x60];
    u8 drawable[0x534 - 0x1ec];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x5b0 - 0x55e];
    u16 delay;
};
void MotionTrack_Initialize(struct MotionTrack *, const void *, const struct MotionTrackVector *,
                            const struct MotionTrackVector *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(u8 *);
void Vanargand_UpdateOffsetMotion(struct Vanargand *p)
{
    void *animation = p->animation;
    Vanargand_SetPendingCue(p, 1);
    if (p->pending) {
        s16 target[3];
        register s16 *v asm("r4");
        s16 *first;
        struct MotionTrack *track;
        s32 x, y, z;
        p->pending = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 4, 3, 4);
        x = p->x;
        y = p->y;
        z = p->z;
        track = (struct MotionTrack *)p->track;
        x -= 256;
        z -= 256;
        v = target;
        asm("" : "+r"(v));
        first = v;
        asm("" : "+r"(first));
        first[0] = x;
        v[1] = y;
        v[2] = z;
        MotionTrack_Initialize(track, &p->x, (struct MotionTrackVector *)v, 0, 4);
        Sound_PlaySong(0x586);
        p->delay = 8;
    }
    MotionTrack_Update(p->track);
    {
        s32 done = 0;
        if (p->trackState == 2)
            done = 1;
        if (done)
            p->finished = 1;
        else {
            u32 mask = 5;
            p->flags |= mask;
        }
    }
}
