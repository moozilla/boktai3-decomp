#include "route_sprite_actor.h"
#include "display_animation.h"
u8 Route_Start(void *, s32, s32, s32);
s32 ScaledShadow_Initialize(u8 *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void RouteSpriteActor_Create(struct ContactPosition *anchor, void *unused, s32 initialTimer,
                             s32 cycles, s32 amount, u32 kind, u32 parameter, u32 option,
                             u32 variant, u32 script, void *context, s32 route)
{
    struct ContactPosition *savedAnchor = anchor;
    s32 savedTimer = initialTimer;
    s32 savedCycles = cycles;
    struct RouteSpritePool *pool;
    struct RouteSpriteActor *actor;
    u32 zero;
    u32 one;
    u8 *graphics;
    pool = gRouteSpritePool;
    if (!pool) {
        pool = RouteSpritePool_Create();
        if (!pool)
            return;
    }
    actor = RouteSpritePool_TryAllocateSlot(pool);
    if (!actor)
        return;
    {
        u8 *state = &actor->state;
        zero = 0;
        one = 1;
        *state = one;
        actor->flags150 = zero;
        actor->frames = zero;
        actor->cyclesRemaining = savedCycles;
        actor->variant = variant;
        actor->amount = amount;
        actor->kind = kind;
        actor->parameter = parameter;
        actor->option = option;
        actor->initialTimer = savedTimer;
        actor->entered = one;
        actor->script = script;
        actor->context = context;
    }
    Route_Start(&actor->route, route, 0, 0);
    {
        s32 shadowResult;
        shadowResult = ScaledShadow_Initialize(actor->shadow, &actor->sprite.position, 64, 16, 96,
                                               100, 0xE00, one, 5, one, zero);
        {
            u32 offset = 0x15B;
            s8 *dst;
            dst = &actor->shadowAbsent;
            *dst = shadowResult;
        }
    }
    {
        struct ContactPosition *dst = &actor->elevatedPosition;
        struct ContactPosition *low;
        struct ContactPosition position;
        low = savedAnchor;
        position = *low;
        *dst = position;
    }
    {
        s16 *dst = &actor->elevatedPosition.y;
        u32 height;
        u32 result;
        u32 value;
        height = 0x2C8;

        result = height;

        value = (u16)*dst;
        *dst = result + value;
    }
    graphics = actor->graphics;
    if (AnimData_Load(graphics, 0xD45E)) {
        Sprite_Init(&actor->sprite, graphics, 0);
        actor->sprite.flags |= one;
        Anim_SetSequence(&actor->sequence, pool->sequenceResource, 0x17, 0, zero);
        AnimData_SelectPaletteBank((void *)((struct DisplaySequenceSprite *)actor)->resource, 0x140);
        {
            struct ContactPosition *low = savedAnchor;
            struct ContactPosition position;
            position = *low;
            actor->sprite.position = position;
        }
        actor->sprite.priority = one;
    }
}
