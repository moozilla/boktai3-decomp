#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *basePosition;
    u8 unknown94[8];
    struct SceneVec origin;
    struct SceneVec target;
    s16 heading, distance, particleAngle;
    u8 unknownB2[0xaa];
    u32 variant, flags, state, timer;
    u8 currentAction, previousAction, reset, enter, finished;
    u8 unknown171[7];
    u32 actionTimer;
    u8 unknown17C[0x20];
    u32 flashTimer;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];

static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct SceneVec *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];
        v->z = Scale(sample * scale);
    }
}
static inline u8 Ended(struct VanargandPart *p)
{
    return p->motion[0x29] == 2;
}

void CpuSet(const void *, void *, u32);
void Palette_InterpolateBanks(void *, const void *, const void *, s32, s32);
// confidence=low: moves to the radial target and fades from the alternate palette.
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
void Vanargand_UpdatePartRevealFade(u8 *boss, struct VanargandPart *p, s32 frame)
{
    u32 mask = 12;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        Vector(&p->origin, p->heading, p->distance);
        p->sprite.flags &= ~1;
        CpuSet(*(void **)((u8 *)p + 0x194), *(void **)((u8 *)p + 0x38), 0x04000008);
        {
            void *resource = *(void **)(boss + 0x4d0);
            Anim_SetSequence(&p->sequence, resource, 9, (u8)p->variant, 0);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->sequence);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
    }
    switch (p->state) {
        case 0:
            Palette_InterpolateBanks(*(void **)((u8 *)p + 0x38), *(void **)((u8 *)p + 0x194),
                                     *(void **)((u8 *)p + 0x190), frame, 6);
            if (frame > 63) {
                u32 next = 1;
                SetState(p, next);
                p->enter = next;
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}
