#include "sprite_trail.h"
#include "scene_pool.h"
struct EffectSourceActor {
    u8 pad[0x28];
    struct TrailVector position;
};
struct OffsetEffectActor {
    u8 pad0[0x268];
    s32 x, y;
    u8 pad270[0x280 - 0x270];
    u32 style;
    u8 pad284[0x3A0 - 0x284];
    s32 sourceIndex;
    u8 pad3A4[0x5CA - 0x3A4];
    s16 parameterA, parameterB;
    u8 pad5CE[0x62C - 0x5CE];
    struct SpriteTrailPool *trailPool;
};
void Sound_PlaySong(u32);
static inline void GetSourcePosition(s32 index, struct TrailVector *sourcePosition)
{
    if (index > 1) {
        sourcePosition->x = 0;
        sourcePosition->y = 0;
        sourcePosition->z = 0;
        return;
    } else {
        struct EffectSourceActor *other = gParticipantActors[index];
        if (other == 0) {
            sourcePosition->x = 0;
            sourcePosition->y = 0;
            sourcePosition->z = 0;
            return;
        } else {
            sourcePosition->x = other->position.x;
            sourcePosition->y = other->position.y;
            sourcePosition->z = other->position.z;
        }
    }
}
u32 ActorEffect_SpawnOffsetEffect(struct OffsetEffectActor *actor)
{
    struct TrailVector position;
    struct TrailVector source;
    position.x = actor->x + 0x28000;
    position.y = actor->y - 0x40000;
    position.z = 0x1C000;
    GetSourcePosition(actor->sourceIndex, &source);
    SpriteTrail_CreateMotionEffect(actor->trailPool, &position, &source, actor->style,
                                   actor->parameterA, actor->parameterB);
    Sound_PlaySong(0x58F);
    /* The original epilogue preserves r0; no explicit result is established. */
}
