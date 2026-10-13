#include "global.h"
struct Vec {
    s16 x, y, z, pad;
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
    u16 id;
    u8 type, entered, state, flags;
    u16 timer, parameter;
    u8 padA, effectActive, animated, remove, frameTimer, frame;
    s16 health;
    u16 speed, ground, pad16;
    struct Vec position;
    void *collider, *attack;
    u8 resource[28];
    struct Sprite sprite;
    u8 anim[16];
};
struct Manager {
    u8 task[24];
    void *animation;
};
extern struct Manager *gUnk_0200046C;
void sub_08220C8C(void *, void *, u32, u32, u32);
void sub_0822B2F8(u32);
void sub_0821FF24(void *, void *, u32);
void sub_0821FE40(void *);
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern u32 gUnk_030053F4;
void sub_0800CC34(void *);
void sub_08014634(u32, u32, void *, void *, void *, u32, u32);
void sub_082260A4(u32);
void FallingObject_UpdateShatter(struct Object *object)
{
    struct PackedVec low, high;
    if (object->entered) {
        object->entered = 0;
        sub_0800CC34(object);
        if (!(gUnk_030053F4 & 2048)) {
            low.x = -15;
            low.y = 8;
            low.z = -15;
            high.x = 30;
            high.y = 22;
            high.z = 30;
            sub_08014634(8, 3, &object->position, &low, &high, 60, 60);
        }
        object->parameter = 30;
    }
    if (object->timer < 12) {
        object->position.y += 8;
        sub_082260A4(40);
    } else {
        object->position.y -= 4;
        if (object->position.y < object->ground)
            object->position.y = object->ground;
    }
    if (object->collider)
        sub_0821FF24(object->collider, &object->position, 0);
    if (object->attack) {
        sub_0821FF24(object->attack, &object->position, 0);
        sub_0821FE40(object->attack);
    }
    if ((object->timer & 7) < 2)
        object->sprite.flags |= 1;
    else
        object->sprite.flags &= ~1;
    if (object->timer++ > object->parameter)
        object->remove = 1;
}
