#include "global.h"
/* Vanargand_UpdateLeapTrack: plays animation 4 with sound 0x587, rides a motion track (origin 0x280/0x300/0x280) spawning afterimages, then plays animation 0xF and once both halves are ready applies them and finishes (provisional name) */
struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
};
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0x9C - 0x54];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[0xAF - 0xA8];
    u8 finished, entered, pendingAnimation, ready;
    u8 padB3[0x18C - 0xB3];
    u8 animation[0x60];
    u8 drawable[0x3FC - 0x1EC];
    struct VanargandPart part;
    u8 pad410[0x500 - 0x410];
    u16 partActive;
    u8 pad502[0x534 - 0x502];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *attachment;
    u8 pad570[0x5B0 - 0x570];
    u16 delay;
    u8 pad5B2[0x5B8 - 0x5B2];
    u16 field5B8, field5BA;
    u8 pad5BC[0xF44 - 0x5BC];
    void *afterimages;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
void Anim_SetFrameDurationScale(void *, u32);
static inline u8 complete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}

static inline void initialize(void *track, void *data, s16 x, s16 y, s16 z, s16 *target, u32 mode,
                              s16 *origin)
{
    s16 *q = origin;
    *q = x;
    origin[1] = y;
    origin[2] = z;
    MotionTrack_Initialize(track, data, origin, target, mode);
}

void Sound_PlaySong(u32);
void VanargandAfterimagePool_Spawn(void *, void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);

void Vanargand_UpdateLeapTrack(struct Vanargand *p)
{
    void *animation = p->animation;
    s16 first[4];
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 1);
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 4, 2, 4);
        Sound_PlaySong(0x587);
        p->delay = 0x1E;
        p->phase = 0;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                initialize(p->track, p->motionData, 0x280, 0x300, 0x280, 0, 4, first);
                p->timer++;
            }
            MotionTrack_Update(p->track);
            if (complete(p)) {
                p->phase = 1;
                p->timer = 0;
            } else
                VanargandAfterimagePool_Spawn(p->afterimages, p->motionData);
            break;
        case 1:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 0xF, 2, 2);
                Anim_SetFrameDurationScale(animation, 0x80);
                p->timer++;
                {
                    struct VanargandPart *part = &p->part;
                    part->state = 7;
                }
                p->partActive = 0;
                p->field5B8 = 0;
                p->field5BA = 0;
            }
            if (p->ready) {
                PairedSceneActor_ApplyToSelectedHalves(p->attachment, 3, 0x19);
                {
                    u8 next = 2;
                    u32 zero = 0;
                    p->phase = next;
                    p->timer = zero;
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
