#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"
// Matched with tools/scope by gpt-6.1-sol workers; the pins and empty asm barriers are
// temporary concessions.
// confidence=low: moves the part along its stored retreat angle, then returns to idle.

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

static inline void AddPlanar(struct SceneVec *v, struct SceneVec *origin)
{
    v->x = v->x + origin->x;
    v->z = v->z + origin->z;
}
// confidence=low: action role inferred from local motion and animation.
void Vanargand_UpdatePartSidestep(u8 *bossArg, struct VanargandPart *p)
{
    u32 mask = 12;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        {
            void *resource = *((void **)(bossArg + 0x4d0));
            Anim_SetSequence(&p->sequence, resource, 11, (u8)p->variant, 0);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->sequence);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
    }
    {
        u32 state = p->state;
        switch (state) {
            case 0: {
                struct SceneVec target;
                u32 entering = p->enter;
                u8 *enter;
                enter = &p->enter;
                if (entering) {
                    *enter = 0;
                    Vector(&target, *(((u8 *)p) + 0x187), 128);
                    AddPlanar(&target, &p->origin);
                    target.y = p->origin.y;
                    MotionTrack_Initialize(p->motion, &p->origin, &target, 0, 4);
                }
                MotionTrack_Update(p->motion);
                if (Ended(p)) {
                    u32 next = 1;
                    p->state = next;
                    p->timer = 0;
                    *enter = next;
                }
                break;
            }

            case 1:
                if (p->enter) {
                    p->enter = 0;
                    {
                        void *resource = *((void **)(bossArg + 0x4d0));
                        Anim_SetSequence(&p->sequence, resource, 9, (u8)p->variant, 0);
                    }
                    PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->sequence);
                }
                p->finished = 1;
                break;
        }
    }
}
