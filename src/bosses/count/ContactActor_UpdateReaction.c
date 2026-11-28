#include "scene/collision/contact_actor.h"
#include "scene/collision/contact_swarm.h"
#include "scene/motion/scene_actor_charge_offset.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void ContactActor_UpdateReaction(struct ContactActorPool *pool, struct ContactActor *slot,
                                 s32 index)
{
    s32 k2 = 0x2D3;
    struct ContactPosition delta;
    struct ContactPosition *write;
    u32 direction;
    u32 speed;
    u32 rawSpeed;
    if (slot->enter) {
        slot->enter = 0;
        ContactCollider_SetFlags(&slot->healthCollider, 4);
        AnimData_SelectPaletteBank(slot->graphics, 0x192);
    }
    rawSpeed = 20 - slot->stateFrame;
    write = &delta;
    direction = slot->direction;
    speed = rawSpeed * 2;
    write->x = SceneActor_ChargeComponent(gUnk_086149C4[(direction + 64) & 255] * speed);
    write->y = 0;
    write->z = SceneActor_ChargeComponent(gUnk_086149C4[direction] * speed);
    {
        struct ContactPosition *position = &slot->position;
        const struct ContactPosition *read = &delta;
        position->x += read->x;
        position->y += read->y;
        position->z += read->z;
    }
    ContactActor_SetDirectionalSequence(pool, slot, 1);
    if (slot->stateFrame > 19) {
        ContactActor_EnterState(slot, 1);
        slot->sprite.flags &= ~1;
    } else {
        if (slot->stateFrame == 6) {
            struct ContactSwarmGraphics *graphics = (struct ContactSwarmGraphics *)slot->graphics;
            AnimData_SelectPaletteBank(graphics, k2);
            graphics->paletteData = pool->tileData;
        }
        if ((slot->stateFrame >> 2) & 1)
            slot->sprite.flags &= ~1;
        else
            slot->sprite.flags |= 1;
        slot->stateFrame++;
    }
}
