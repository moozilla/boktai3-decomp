#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#include "engine.h"
#include "display_sequence_inline.h"
#include "scene_actor_terrain.h"
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

struct VanargandBoss {
    u8 unk_00[0x4d0];
    void *partSequenceResource;
};
static inline s32 TerrainHeight(struct SceneVec *v)
{
    const struct ContactPosition *position = (const struct ContactPosition *)v;
    return SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(position), position);
}

static inline void SetY(struct SceneVec *v, s32 y)
{
    v->y = y;
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
static inline void AddFlags(struct VanargandPart *p, u32 flags)
{
    p->flags |= flags;
}
static inline void SetVector(struct SceneVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

// Tentative action: raise the part along a variant-dependent heading, then land.
void Vanargand_UpdatePartLeap(struct VanargandBoss *boss, struct VanargandPart *p)
{
    u8 *enter;
    u32 mask = 12;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        {
            void *resource = boss->partSequenceResource;
            Anim_SetSequence(&p->sequence, resource, 9, (u8)p->variant, 0);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->sequence);
        SetState(p, 0);
        p->enter = 1;
    }
    switch (p->state) {
        case 0: {
            struct SceneVec target;
            u32 entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                if (p->variant == 0)
                    Vector(&target, 48, 1024);
                else
                    Vector(&target, 16, 1024);
                target.y = 768;
                MotionTrack_Initialize(p->motion, &p->origin, &target, 0, 3);
            }
            MotionTrack_Update(p->motion);
            if (Ended(p)) {
                u32 next = 1;
                SetState(p, next);
                *enter = next;
            }
            break;
        }
        case 1: {
            struct SceneVec target;
            struct SceneVec speed;
            u32 entering = p->enter;
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                target = p->origin;
                SetY(&target, TerrainHeight(&p->origin));
                SetVector(&speed, 8, 32, 8);
                MotionTrack_Initialize(p->motion, &p->origin, &target, 0, 1);
            }
            MotionTrack_Update(p->motion);
            if (Ended(p)) {
                p->finished = 1;
                SetState(p, 2);
                *enter = 1;
            }
        }
        case 2:
            AddFlags(p, 128);
            p->finished = 1;
            break;
    }
}
