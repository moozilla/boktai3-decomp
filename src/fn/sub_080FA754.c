#define gHoverSineTable gUnk_086149C4
#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
void SceneActorControl_UpdateHoverHeight(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    u32 mask = 0x8000;
    if (!(actor->flags2D4 & mask)) {
        s32 height = Scale(gHoverSineTable[context[0xEAA]] << 5);
        u8 *scene = (*(u8 **)gPlayers);
        u32 y = height + 220;
        y += *(u16 *)(scene + 0x32);
        actor->body.position.y = y;
        *(u16 *)(context + 0xA56) = y;
        context[0xEAA] += 2;
    }
}
