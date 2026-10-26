// MATCHING_CONCESSION: register-pinning
#include "route_sprite_actor.h"
u8 Route_Start(void *, s32, s32, s32);
s32 ScaledShadow_Initialize(u8 *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void RouteSpriteActor_Create(struct ContactPosition *anchor, void *unused, s32 initialTimer,
                             s32 cycles, s32 amount, u32 kind, u32 parameter, u32 option,
                             u32 variant, u32 script, void *context, s32 route)
{
    struct ContactPosition *savedAnchor = anchor;
    s32 savedTimer = initialTimer;
    s32 savedCycles = cycles;
    register struct RouteSpritePool *pool asm("r6");
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
    /* Offset temporaries preserve allocation lifetimes for the typed fields
     * listed in RouteSpriteActor; they do not encode instruction bodies. */
    {
        register u32 stateOffset asm("r1") = 0x152;
        register u8 *state asm("r0");
        register u32 zeroLow asm("r2");
        u32 initialOne = 1;
        register u32 offsetA asm("r3");
        register u32 offsetB asm("r2");
        asm("" : "+r"(stateOffset));
        state = (void *)((u32)actor + stateOffset);
        zeroLow = 0;
        asm("" : "+r"(zeroLow));
        zero = zeroLow;
        one = initialOne;
        asm("" : "+r"(one));
        *state = one;
        offsetA = 0x150;
        *(u16 *)((u32)actor + offsetA) = zero;
        offsetB = 0x154;
        *(u16 *)((u32)actor + offsetB) = zero;
        offsetA += 0x18;
        *(s16 *)((u32)actor + offsetA) = savedCycles;
        actor->variant = variant;
        offsetB += 0x10;
        *(s16 *)((u32)actor + offsetB) = amount;
        offsetA -= 0xB;
        *(u8 *)((u32)actor + offsetA) = kind;
        actor->parameter = parameter;
        offsetB -= 6;
        *(u8 *)((u32)actor + offsetB) = option;
        offsetA += 0xD;
        *(s16 *)((u32)actor + offsetA) = savedTimer;
        offsetB -= 8;
        {
            register u8 *dst asm("r0") = (void *)((u32)actor + offsetB);
            *dst = one;
        }
        offsetA += 6;
        {
            register u32 *dst asm("r1") = (void *)((u32)actor + offsetA);
            *dst = script;
        }
        {
            register u32 offset asm("r0") = 0x174;
            register void **dst asm("r1");
            asm("" : "+r"(offset));
            dst = (void *)((u32)actor + offset);
            *dst = context;
        }
    }
    Route_Start(&actor->route, route, 0, 0);
    {
        s32 shadowResult;
        shadowResult = ScaledShadow_Initialize(actor->shadow, &actor->sprite.position, 64, 16, 96,
                                               100, 0xE00, one, 5, one, zero);
        {
            u32 offset = 0x15B;
            s8 *dst;
            dst = (void *)((u32)actor + offset);
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
        register u32 height asm("r2");
        register u32 result asm("r0");
        register u32 value asm("r3");
        height = 0x2C8;
        asm("" : "+r"(height));
        result = height;
        asm("" : "+r"(result));
        value = (u16)*dst;
        *dst = result + value;
    }
    graphics = actor->graphics;
    if (AnimData_Load(graphics, 0xD45E)) {
        Sprite_Init(&actor->sprite, graphics, 0);
        actor->sprite.flags |= one;
        Anim_SetSequence(&actor->sequence, pool->sequenceResource, 0x17, 0, zero);
        AnimData_SelectPaletteBank(*(void **)&actor->sprite.unknown8[4], 0x140);
        {
            struct ContactPosition *low = savedAnchor;
            struct ContactPosition position;
            position = *low;
            actor->sprite.position = position;
        }
        actor->sprite.priority = one;
    }
}
