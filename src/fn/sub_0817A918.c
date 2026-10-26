#include "global.h"
#include "engine.h"
struct ActorPathPosition {
    s16 x, y, z, pad;
};
struct ActorPathState {
    u8 unknown0[0x54];
    struct ActorPathPosition position;
    u8 unknown5C[0x22C];
    u8 pathContext[1];
};
struct PathPlayer {
    u8 unknown0[0x30];
    struct ActorPathPosition position;
};
extern struct PathPlayer *gUnk_02000580[];
extern const u16 gUnk_0203B400[];
s32 Nav_IsPositionInMesh(void *, struct ActorPathPosition *, struct ActorPathPosition *);
void ProximityVolume_TranslateBounds(void *, struct ActorPathPosition *);
s32 Mod(s32, s32);
s32 ActorPosition_TestPath(struct ActorPathState *p, struct ActorPathPosition *destination);
static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
static inline void SetVec(struct ActorPathPosition *v, s16 x, s16 y, s16 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
static inline struct ActorPathPosition *
AddVec(struct ActorPathPosition *dst, struct ActorPathPosition *a, struct ActorPathPosition *b)
{
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
    dst->z = a->z + b->z;
    return dst;
}
// confidence=low: picks a random reachable corner (+-0x100 in x/z) around player 0 and paths toward it.
s32 sub_0817A918(struct ActorPathState *p)
{
    struct ActorPathPosition candidates[4];
    struct ActorPathPosition base;
    s32 i, n;
    struct ActorPathState *self = p;
    base = gUnk_02000580[0]->position;
    SetVec(&candidates[0], 0x100, 0, 0x100);
    SetVec(&candidates[1], 0x100, 0, -0x100);
    SetVec(&candidates[2], -0x100, 0, 0x100);
    SetVec(&candidates[3], -0x100, 0, -0x100);
    ProximityVolume_TranslateBounds(p->pathContext, &p->position);
    n = 0;
    for (i = 0; i < 4; i++) {
        AddVec(&candidates[n], &candidates[i], &base);
        if (Nav_IsPositionInMesh(p->pathContext, &self->position, &candidates[n]) == 1)
            n++;
    }
    if (n <= 0)
        return 0;
    i = Mod(Random(), n);
    return ActorPosition_TestPath(p, &candidates[i]);
}
