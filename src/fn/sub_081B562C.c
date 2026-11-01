#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#include "render.h"
#include "player_cue_sequence.h"
// Exact with register-pinning and empty-asm concessions.
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    struct ContactSwarmGraphics graphics;
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
    u8 unknown17C[0x14];
    void *paletteSource; /* Tentative: source of the 32-byte graphics palette copy. */
    u8 unknown194[8];
    u32 flashTimer;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];

static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct SceneVec *v, s32 angle, s16 scale)
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

void CpuSet(const void *, void *, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void Sound_PlaySong(u32);
void Vanargand_SpawnPartEmergenceEffects(void *);
static inline s32 Divide32(s32 value)
{
    return value >= 0 ? value >> 5 : -((-value) >> 5);
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}

struct EmergenceProjection {
    s16 x, y, z;
} __attribute__((aligned(4)));
static inline void Project(struct EmergenceProjection *out, struct SceneVec *position)
{
    s32 x = position->x, z = position->z;
    s32 sum, height;
    out->x = Divide32((x - z) * 3);
    sum = Divide32((x + z) * 3);
    height = Divide32(position->y * 3);
    out->y = sum - height;
    out->z = sum + height;
}
// confidence=low: reveals the controller and moves its part outward after two keyframe tracks.
#define Vanargand_UpdatePartEmergence sub_081B562C
void Vanargand_UpdatePartEmergence(u8 *boss, struct VanargandPart *p)
{
    struct EmergenceProjection projected;
    u8 *enter;
    u8 *keyframes;
    u8 *animation;
    u32 entering;
    u32 state;
    u32 mask = 12;
    void *paletteSource;
    void *paletteDestination;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        if (!(p->sprite.flags & 1)) {
            p->finished = 1;
            SetState(p, 3);
            p->enter = 1;
            return;
        }
        *((u32 *)(boss + 0x20)) &= ~1;
        if (p->variant == 0)
            *((u32 *)(boss + 0x20)) |= 4;
        else
            *((u32 *)(boss + 0x20)) &= ~4;
        {
            void *data = *((void **)(boss + 0x4d0));
            u32 zero = 0;
            Anim_SetSequence(&p->sequence, data, 9, (u8)p->variant, zero);
        }
        PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->sequence);
        paletteSource = p->paletteSource;
        paletteDestination = (void *)p->graphics.paletteData;
        CpuSet(paletteSource, paletteDestination, 0x04000008);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
    }
    state = p->state;
    switch (state) {
        case 0:
            entering = p->enter;
            enter = &p->enter;
            animation = boss + 0x18;
            keyframes = boss + 0x78;
            if (entering) {
                p->enter = 0;
                Anim_SetKeyframes(animation, keyframes, 0, 2, 0);
                Sound_PlaySong(0x58d);
            }
            if (Anim_Step(animation, keyframes)) {
                SetState(p, 1);
                *enter = 1;
            }
            p->sprite.position = *((struct ContactPosition *)p->basePosition);
            break;

        case 1:
            entering = p->enter;
            enter = &p->enter;
            animation = boss + 0x18;
            keyframes = boss + 0x78;
            if (entering) {
                p->enter = 0;
                Anim_SetKeyframes(animation, keyframes, 1, 2, 0);
            }
            if (Anim_Step(animation, keyframes)) {
                SetState(p, 2);
                *enter = 1;
                *((u32 *)(boss + 0x20)) |= 1;
            }
            p->sprite.position = *((struct ContactPosition *)p->basePosition);
            break;

        case 2: {
            struct SceneVec target;
            entering = p->enter;
            enter = &p->enter;
            if (entering) {
                p->enter = 0;
                p->sprite.flags &= ~1;
                Vector(&p->origin, p->heading, 640);
                Vector(&target, p->heading, p->distance);
                MotionTrack_Initialize(p->motion, &p->origin, &target, 0, 4);
                Vanargand_SpawnPartEmergenceEffects(p);
                Sound_PlaySong(0x58e);
            }
            MotionTrack_Update(p->motion);
            if (Ended(p)) {
                SetState(p, 3);
                *enter = 1;
            }
            break;
        }

        case 3:
            p->finished = 1;
            break;
    }

    Project(&projected, p->basePosition);
    projected.x = (projected.x - gCameraOrigin[0]) + 120;
    projected.y = (projected.y - gCameraOrigin[1]) + 92;
    *((struct EmergenceProjection *)(boss + 0x38)) = projected;
}
