#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
#define TetherActor_UpdateAuxiliaryTransition sub_0804C748
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_RemoveAuxiliarySprite(struct TetherActor *);
void TetherActor_EmitPointEffect(struct ContactPosition *, void *);
void DisplayAnimation_SetFlags(struct DisplayAnimation *, u32);
void DisplayAnimation_ClearFlags(struct DisplayAnimation *, u32);
u8 *sub_0821D64C(s32, s32);
extern struct TetherActor *gUnk_02000488;
static inline u8 *propertiesAt(s32 x, s32 z)
{
    s32 cell;
    u8 *properties;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    properties = sub_0821D64C(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}
void TetherActor_UpdateAuxiliaryTransition(struct TetherActor *actor)
{
    u32 visible;
    u32 emit;
    struct DisplayAnimation *display;
    struct DisplayAnimation *activeDisplay;
    register u16 *frame asm("r4");
    register u32 rawFrame asm("r2");
    u32 highFrame;
    u32 current;
    if (actor->state != 5)
        TetherActor_SetState(actor, 5);
    if (gUnk_030053F4 & 512)
        return;
    actor->glowMode = 2;
    actor->heading = 0;
    actor->chargeCollider.flags |= 4;
    visible = 0;
    emit = visible;
    frame = &actor->stateFrame;
    rawFrame = *frame;
    highFrame = rawFrame << 16;
    current = highFrame >> 16;
    if (current <= 63) {
        actor->position.y = (actor->position.y * 7 + actor->auxiliaryOrigin.y) >> 3;
        goto show;
    }
    if (current <= 127) {
        s32 shake;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 bit = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
            bit &= 1;
            shake = (bit << 4) + bit - 8;
        }
        actor->shakeX = shake;
        actor->shakeZ = -shake;
        visible = 1;
        if ((*frame & 15) == 0)
            emit = visible;
    } else if (current <= 191) {
        if ((highFrame >> 17) & 3)
            visible = 1;
        if ((rawFrame & 15) == 0)
            emit = 1;
    } else {
        if ((highFrame >> 17) & 1)
            visible = 1;
        if ((rawFrame & 15) == 0)
            emit = 1;
    }
    if (visible) {
show:
        activeDisplay = &actor->display.animation;
        DisplayAnimation_ClearFlags(activeDisplay, 1);
        actor->auxiliarySprite.flags &= ~1;
        {
            struct ContactPosition *position = &actor->auxiliarySprite.position;
            s32 x, z;
            u8 *properties;
            u32 surface;
            s32 height;
            x = position->x >> 8;
            z = position->z >> 8;
            display = activeDisplay;
            properties = propertiesAt(x, z);
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
            case 1: height -= (u8)position->z; break;
            case 2: height -= (u8)position->x; break;
            }
            actor->auxiliarySprite.position.y = height;
        }
    } else {
        activeDisplay = &actor->display.animation;
        DisplayAnimation_SetFlags(activeDisplay, 1);
        {
            u32 *flags = &actor->auxiliarySprite.flags;
            register u32 value asm("r0") = *flags;
            value |= 1;
            *flags = value;
        }
        display = activeDisplay;
    }
    if (emit)
        TetherActor_EmitPointEffect(&actor->position, actor->unknown43C);
    TetherActor_SetEntry(actor, 0);
    if (++actor->stateFrame > 255) {
        TetherActor_RemoveAuxiliarySprite(actor);
        TetherActor_SetState((struct TetherActor *)gUnk_02000488, 0);
        if (actor->interactionValue) {
            u32 script = actor->interactionValue;
            actor->interactionValue = 0;
            Script_Call(script, 0);
            actor->sampleRequested = 1;
            DisplayAnimation_SetFlags(display, 1);
        } else {
            DisplayAnimation_SetFlags(display, 1);
            actor->sampleRequested = 1;
        }
    }
}
