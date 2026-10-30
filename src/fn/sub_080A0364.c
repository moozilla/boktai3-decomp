#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "display_animation.h"

extern const s16 gUnk_086149C4[];

struct CrowHeightState {
    u8 unknown0[0x3EC];
    u8 bobPhase;
};
struct CrowHeightDrawable {
    u8 unknown0[0x1E];
    u16 offsetY;
    u8 unknown20[0x28];
    struct DisplaySequenceState sequence;
};
static inline u8 Blocked(struct SceneActorRegistryResetView *s)
{
    if (!s)
        goto zero;
    {
        u32 mask = 0x40;
        if (s->flags2D4 & mask)
            goto one;
    }
    {
        u32 mask = 0x20000;
        if (s->flags2D0 & mask)
            goto one;
    }
    if (s->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
void Crow_UpdateGroundHeight(struct SceneActorRegistryResetView *actor)
{
    struct CrowHeightState *state = actor->context;
    u32 kind;
    if (Blocked(actor))
        return;
    kind = actor->field2B5;
    if (kind == 0 || kind == 0x25 || kind == 0x26) {
        actor->body.position.y = actor->field2EA;
        return;
    }
    {
        u32 mask = 0x100000;
        if (actor->flags2D0 & mask) {
            if (kind != 0x23)
                goto adjust;
            {
                s32 cell = SceneActorTerrain_GetCellIndex(&actor->body.position);
                s32 height = (*SceneActorTerrain_GetCell(cell) & 15) << 8;
                if ((s16)actor->field2EA < height + 0x118) {
                    actor->field2EA += Scale12(gUnk_086149C4[state->bobPhase] * 8);
                    if (state->bobPhase <= 0x3F)
                        state->bobPhase++;
                }
            }
        }
    }
    actor->body.position.y = actor->field2EA;
    return;
adjust: {
    s32 diff;
    s32 magnitude;
    u32 ground;
    s32 bias;
    ground = actor->groundHeight2BE;
    bias = -0x118;
    diff = ground - (actor->body.position.y + bias);
    magnitude = diff;
    if (magnitude < 0)
        magnitude = -magnitude;
    if (magnitude > 0x10) {
        if (diff < 0)
            actor->field2EA -= 2;
        else
            actor->field2EA += 2;
    }
}
    actor->body.position.y = actor->field2EA;
    if ((u8)(actor->field2B5 - 1) <= 1) {
        struct DisplaySequenceState *seq;
        struct CrowHeightDrawable *drawable;
        drawable = actor->drawable;
        seq = &drawable->sequence;
        switch (seq->index) {
            case 0:
                drawable->offsetY = drawable->offsetY + 0x18;
                break;
            case 1: {
                u32 base = drawable->offsetY + 0x18;
                drawable->offsetY = base + (seq->elapsed << 2);
                break;
            }
            case 2: {
                u32 base = drawable->offsetY + 0x3E;
                drawable->offsetY = base - seq->elapsed * 12;
                break;
            }
            case 3: {
                u32 base = drawable->offsetY + 2;
                drawable->offsetY = base - (seq->elapsed << 3);
                break;
            }
            case 4: {
                u32 base = drawable->offsetY - 0x16;
                drawable->offsetY = base + seq->elapsed * 7;
                break;
            }
        }
    }
    return;
}
