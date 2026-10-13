#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Sprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 tail[8];
};
struct Object {
    u8 task[32];
    void *scene;
    s16 health;
    u16 maximum, cooldown, phase, timer, flash;
    u8 rising, pad31;
    s16 baseY, offset;
    u16 count, kind, pad3A;
    u8 resource[28];
    struct Sprite sprite;
    u8 shadow[64];
    u16 collision[4];
};
s32 sub_0821ABA8(u32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void *sub_0822C034(void *);
void sub_0821FEB4(void *, u32, u32, u32, u32, void *, void *);
void sub_0821FF84(void *, void *, void *);
void sub_0821FF24(void *, void *, u32);
void sub_0821FE40(void *);
void sub_082151E4(void *, u32);
void sub_082144A4(void *, void *, u32);
void sub_08215284(void *, u32);
void Shadow_Initialize(void *, void *, u32);
void EffectEmitter_ReceiveHit(void);
s32 EffectEmitter_Initialize(struct Object *object, u32 id)
{
    struct PackedVec position, size, offset;
    void *collision;
    void *resource;
    struct Sprite *sprite;
    u32 zero = 0;
    *(u16 *)&object->task[24] = id;
    object->maximum = sub_0821ABA8(108, 200);
    object->health = 1;
    object->cooldown = zero;
    object->count = sub_0821ABA8(110, 4);
    object->kind = sub_0821ABA8(101, 50);
    object->phase = zero;
    object->timer = zero;
    object->flash = zero;
    if (Script_SeekToKeyword(112)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue() + 100;
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    object->scene = sub_0822C034(&position);
    object->rising = 0;
    object->baseY = ((struct Vec *)&position)->y;
    object->offset = 0;
    size.x = 30;
    size.y = 320;
    size.z = 30;
    offset.x = 0;
    offset.y = 160;
    offset.z = 0;
    collision = object->collision;
    sub_0821FEB4(collision, 0, 0x4001, 0, 16, &size, &offset);
    sub_0821FF84(collision, EffectEmitter_ReceiveHit, object);
    sub_0821FF24(collision, &position, 0);
    sub_0821FE40(collision);
    resource = object->resource;
    sub_082151E4(resource, 1642);
    sprite = &object->sprite;
    sub_082144A4(sprite, resource, 0);
    sprite->frame = 0;
    sub_08215284(resource, 486);
    object->sprite.position = *(struct Vec *)&position;
    Shadow_Initialize(object->shadow, &object->sprite.position, 0);
    return 0;
}
