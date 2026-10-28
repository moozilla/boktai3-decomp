#include "global.h"
#include "scene_pool.h"
struct EffectPosition {
    s32 x, y, z, unused;
};
struct EffectSourceActor {
    u8 pad[0x28];
    struct EffectPosition position;
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
    u32 effectId;
};
void SpriteTrail_CreateMotionEffect(u32, struct EffectPosition *, s32 *, u32, s32, s32);
void Sound_PlaySong(u32);
static inline void GetSourcePosition(s32 index, s32 *sourcePosition)
{
    if (index > 1) {
        sourcePosition[0] = 0;
        sourcePosition[1] = 0;
        sourcePosition[2] = 0;
        return;
    } else {
        struct EffectSourceActor *other = gParticipantActors[index];
        if (other == 0) {
            sourcePosition[0] = 0;
            sourcePosition[1] = 0;
            sourcePosition[2] = 0;
            return;
        } else {
            sourcePosition[0] = other->position.x;
            sourcePosition[1] = other->position.y;
            sourcePosition[2] = other->position.z;
        }
    }
}
u32 ActorEffect_SpawnOffsetEffect(struct OffsetEffectActor *actor)
{
    struct EffectPosition position;
    s32 source[4];
    position.x = actor->x + 0x28000;
    position.y = actor->y - 0x40000;
    position.z = 0x1C000;
    GetSourcePosition(actor->sourceIndex, source);
    SpriteTrail_CreateMotionEffect(actor->effectId, &position, source, actor->style,
                                   actor->parameterA, actor->parameterB);
    Sound_PlaySong(0x58F);
    /* The original epilogue preserves r0; no explicit result is established. */
}
