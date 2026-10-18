#define gRatatoskPlayerOffsets gUnk_0824FDD0
// Checker ELF predates promotion of these two names.
#include "global.h"
#include "scene_vec.h"

struct RatatoskPlayerOffsets {
    struct SceneVec points[4];
};
extern const struct RatatoskPlayerOffsets gUnk_0824FDD0;
struct SceneVec *Player_GetEffectivePosition(s32);
struct SceneVec *Ratatosk_GetPosition(void);

static inline void AddPosition(struct SceneVec *out, const struct SceneVec *a,
                               const struct SceneVec *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

void Ratatosk_GetPlayerOffsetNearestSelf(void *unused, struct SceneVec *out)
{
    s32 selected = -1;
    struct RatatoskPlayerOffsets offsets = gRatatoskPlayerOffsets;
    struct SceneVec candidate;
    struct SceneVec result;
    u32 minimum = -1;
    s32 i;
    struct SceneVec *player;
    struct SceneVec *self;
    for (i = 0; i < 4; i++) {
        s32 dx, dz;
        u32 distance;
        AddPosition(&candidate, &offsets.points[i], Player_GetEffectivePosition(0));
        self = Ratatosk_GetPosition();
        dx = candidate.x - self->x;
        dz = candidate.z - self->z;
        distance = dx * dx + dz * dz;
        if (distance <= minimum) {
            minimum = distance;
            selected = i;
        }
    }
    AddPosition(&result, &offsets.points[selected], Player_GetEffectivePosition(0));
    *out = result;
}
