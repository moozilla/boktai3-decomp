#include "damage_controller.h"

void DamageController_AccumulateHit(struct DamageCollider *attack, struct DamageCollider *target,
                                    struct DamageController *controller)
{
    s32 kind = 0, flags;
    if (controller->flags & 8)
        kind = 1;
    else if (controller->flags & 2) {
        if (controller->kindMask == attack->kind)
            kind = 1;
    } else if (controller->kindMask & attack->kind)
        kind = 1;
    flags = 0;
    if (controller->flags & 16)
        flags = 1;
    else if (controller->flags & 4) {
        if (controller->flagMask == attack->flags)
            flags = 1;
    } else if (controller->flagMask & attack->flags)
        flags = 1;
    if (kind && flags) {
        target->damage += attack->damage;
        controller->hitPosition = attack->position;
    }
}

s32 DamageController_Update(struct DamageController *controller)
{
    /* The original stores these coordinates but passes separate, uninitialized
 * script descriptors. Preserve this apparent original-code bug. */
    volatile s32 values[3];
    struct DamageCollider *collider = &controller->collider;
    if (controller->cooldown) {
        controller->cooldown--;
        controller->collider.state |= 4;
    } else
        controller->collider.state &= ~4;
    if (collider->damage) {
        s32 deathArgs[2], hitArgs[2];
        controller->cooldown = controller->delay;
        controller->health -= collider->damage;
        if (controller->health < 0) {
            if (controller->deathScript) {
                values[0] = controller->hitPosition.x;
                values[1] = controller->hitPosition.y;
                values[2] = controller->hitPosition.z;
                Script_Call(controller->deathScript, deathArgs);
            }
            if (controller->flags & 1)
                Task_Kill(controller);
            else
                controller->health = controller->maximum;
        } else if (controller->hitScript) {
            values[0] = controller->hitPosition.x;
            values[1] = controller->hitPosition.y;
            values[2] = controller->hitPosition.z;
            Script_Call(controller->hitScript, hitArgs);
        }
        collider->damage = 0;
    }
    return 0;
}

u32 sub_0800C954(struct DamageController *controller)
{
    Collider_Remove(&controller->collider);
    return 0;
}

s32 DamageController_Initialize(struct DamageController *controller, u32 id)
{
    struct DamagePackedVec position, size, offset;
    struct DamageCollider *collider;
    controller->id = id;
    controller->maximum = Script_GetKeywordValue(108, 100);
    controller->health = controller->maximum;
    controller->height = Script_GetKeywordValue(83, 16);
    if (Script_SeekToKeyword(112)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    if (Script_SeekToKeyword(115)) {
        size.x = Script_GetValue();
        size.y = Script_GetValue();
        size.z = Script_GetValue();
    } else {
        size.x = 0;
        size.y = 0;
        size.z = 0;
    }
    if (Script_SeekToKeyword(111)) {
        offset.x = Script_GetValue();
        offset.y = Script_GetValue();
        offset.z = Script_GetValue();
    } else {
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
    }
    collider = &controller->collider;
    Collider_Init(collider, (u16)id, 0x4001, 0, (u16)controller->height, &size, &offset);
    Collider_SetPosition(collider, &position, 0);
    Collider_SetCallback(collider, DamageController_AccumulateHit, controller);
    sub_0821FF7C(collider, 0, 0, 0);
    Collider_Refresh(collider);
    controller->flags = Script_GetKeywordValue(102, 0);
    if (Script_SeekToKeyword(119)) {
        controller->kindMask = Script_GetValue();
    } else {
        controller->kindMask = 0;
        controller->flags |= 8;
    }
    if (Script_SeekToKeyword(97)) {
        controller->flagMask = Script_GetValue();
    } else {
        controller->flagMask = 0;
        controller->flags |= 16;
    }
    controller->hitScript = Script_GetKeywordValue(68, 0);
    controller->deathScript = Script_GetKeywordValue(100, 0);
    controller->cooldown = 0;
    controller->delay = Script_GetKeywordValue(105, 0);
    return 0;
}

void *sub_0800CB60(u32 arg)
{
    void *obj;
    obj = Task_Create(8, 0x90);
    if (obj != 0) {
        Task_SetCallbacks(obj, DamageController_Update, sub_0800C954);
        if (DamageController_Initialize(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
