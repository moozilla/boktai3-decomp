// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
u8 *sub_0821D64C(s32, s32);
u32 sub_08225AB4(void *, u32, const struct ContactPosition *, u32, u32, void *);
u32 sub_08225B28(void *, void *);
u32 sub_08225B30(void *, void *);
u32 sub_08072BF0(void *, void *, u32);
void sub_0821BC68(void *, void *);
void sub_08003C00(void *);
void TetherActor_InitBodyFromScript(struct TetherActor *actor)
{
    s32 found = Script_SeekToKeyword('p');
    struct ContactPosition *position;
    s32 x, z, cell;
    u8 *properties;
    u32 surface, height;
    register struct TetherBodyConfig *initialConfig asm("r4");
    struct TetherBodyConfig *config;
    u32 displayKind;
    if (found) {
        actor->savedPosition.x = Script_GetValue();
        actor->savedPosition.y = Script_GetValue();
        actor->savedPosition.z = Script_GetValue();
    } else {
        actor->savedPosition.x = found;
        actor->savedPosition.y = found;
        actor->savedPosition.z = found;
    }
    position = &actor->savedPosition;
    x = position->x >> 8;
    z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else {
        u32 row = gUnk_030052F4->rows[z];
        cell = row + x;
    }
    properties = sub_0821D64C(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    actor->savedPosition.y = height;

    *(struct ContactPosition *)actor->display.animation.position = actor->savedPosition;
    actor->origin = actor->savedPosition;
    actor->targetPosition = actor->savedPosition;
    initialConfig = &actor->bodyConfig;
    sub_08225AB4(initialConfig, actor->entry, &actor->savedPosition, 0, 6, actor);
    displayKind = actor->display.animation.kind;
    config = initialConfig;
    if (displayKind == 0)
        goto displayDone;
    if (displayKind == 1)
        sub_08225B28(config, (u8 *)&actor->display.animation.resource + 0x1C);
    else if (displayKind == 2)
        sub_08225B30(config, (u8 *)&actor->display.animation.resource + 0x20);
displayDone:
    actor->bodyConfig.angle = ((actor->heading + 5) & 7) << 5;
    actor->interactionProbe.bodyId = actor->bodyConfig.id;
    if (actor->kind != 3) {
        if (!actor->kindValue)
            sub_08072BF0(actor->auxiliaryRegistration, config, 1);
        else
            sub_08072BF0(actor->auxiliaryRegistration, config, 4);
        actor->auxiliaryRegistered = 1;
    }
    if (actor->kind == 9) {
        sub_0821BC68(&actor->movementProbeStorage, &actor->position);
        actor->movementProbe = &actor->movementProbeStorage;
    }
    sub_08003C00(config);
}
