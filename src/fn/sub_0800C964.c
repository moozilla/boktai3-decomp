#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Collider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[54];
    u16 damage;
    u8 pad40[20];
};
struct Controller {
    u8 task[24];
    struct Collider collider;
    u16 id;
    s16 health;
    u16 maximum, flags;
    u32 kindMask, flagMask;
    u8 pad7c[4];
    u16 cooldown, delay, hitScript, deathScript;
    struct Vec hitPosition;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
s32 sub_0821ABA8(u32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void sub_0821FEB4(void *, u32, u32, u32, u32, void *, void *);
void sub_0821FF24(void *, void *, u32);
void sub_0821FF84(void *, void *, void *);
void sub_0821FF7C(void *, u32, u32, u32);
void sub_0821FE40(void *);
void sub_0800C7F4(void);
#define DamageController_Initialize sub_0800C964
s32 DamageController_Initialize(struct Controller *controller, u32 id)
{
    struct PackedVec position, size, offset;
    struct Collider *collider;
    controller->id = id;
    controller->maximum = sub_0821ABA8(108, 100);
    controller->health = controller->maximum;
    *(u32 *)controller->pad7c = sub_0821ABA8(83, 16);
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
    sub_0821FEB4(collider, (u16)id, 0x4001, 0, (u16) * (u32 *)controller->pad7c, &size, &offset);
    sub_0821FF24(collider, &position, 0);
    sub_0821FF84(collider, sub_0800C7F4, controller);
    sub_0821FF7C(collider, 0, 0, 0);
    sub_0821FE40(collider);
    controller->flags = sub_0821ABA8(102, 0);
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
    controller->hitScript = sub_0821ABA8(68, 0);
    controller->deathScript = sub_0821ABA8(100, 0);
    controller->cooldown = 0;
    controller->delay = sub_0821ABA8(105, 0);
    return 0;
}
