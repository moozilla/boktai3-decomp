#include "global.h"
#include "render.h"

struct ShadowPosition {
    s16 x, y, z;
    u16 unk_06;
};
struct ShadowActor {
    u8 unk_00[8];
    u8 animationData[0x20];
    u8 animation[8];
    u32 animationFlags;
    u8 unk_34[0x14];
    struct ShadowPosition position;
    u8 unk_50[0x150 - 0x50];
    void *pool;
    u8 entered, phase, animationEnded, state;
};
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void SceneEntryPool_CreateEntry(void *, struct ShadowPosition *, u32);
s32 __divsi3(s32, s32);

static inline s32 ShiftTowardZero(s32 value, s32 shift)
{
    s32 result;
    if (value >= 0)
        result = value >> shift;
    else
        result = -((-value) >> shift);
    return result;
}

static inline void ProjectPosition(struct ShadowPosition *out, struct ShadowPosition *in)
{
    s32 a, b, c, d, x = in->x, z = in->z;
    out->x = ShiftTowardZero(3 * (x - z), 5);
    a = ShiftTowardZero(3 * (x + z), 5);
    b = ShiftTowardZero(3 * in->y, 5);
    c = a - b;
    d = a + b;
    out->x = out->x - (*(struct ShadowPosition *)gCameraOrigin).x + 120;
    out->y = c - (*(struct ShadowPosition *)gCameraOrigin).y + 80;
    out->z = d - (*(struct ShadowPosition *)gCameraOrigin).z;
}

static inline void UnprojectPosition(struct ShadowPosition *out, struct ShadowPosition *in, s32 y)
{
    s32 a, b, c;
    a = in->x + (*(struct ShadowPosition *)gCameraOrigin).x - 120;
    b = in->y + (*(struct ShadowPosition *)gCameraOrigin).y - 80;
    c = __divsi3((a + b) << 5, 3) + y;
    out->x = ShiftTowardZero(c, 1);
    out->y = y;
    c = __divsi3((b - a) << 5, 3) + y;
    out->z = ShiftTowardZero(c, 1);
}

static inline void ChangeState(struct ShadowActor *actor, u32 state)
{
    actor->state = state;
    actor->entered = 0;
    actor->phase = 0;
}

void SceneEffect_UpdateRingSpawn(struct ShadowActor *actor)
{
    if (!actor->entered) {
        Anim_SetKeyframes(actor->animation, actor->animationData, 3, 2, 0);
        if (actor->animationFlags & 4) {
            struct ShadowPosition screen, world;
            s32 y;
            ProjectPosition(&screen, &actor->position);
            y = actor->position.y + 213;
            screen.x -= 39;
            screen.y -= 20;
            UnprojectPosition(&world, &screen, y);
            SceneEntryPool_CreateEntry(actor->pool, &world, 1);
        } else {
            struct ShadowPosition screen, world;
            s32 y;
            ProjectPosition(&screen, &actor->position);
            y = actor->position.y + 213;
            screen.x += 39;
            screen.y -= 20;
            UnprojectPosition(&world, &screen, y);
            SceneEntryPool_CreateEntry(actor->pool, &world, 0);
        }
        actor->entered++;
    } else if (actor->animationEnded) {
        ChangeState(actor, 5);
    }
}
