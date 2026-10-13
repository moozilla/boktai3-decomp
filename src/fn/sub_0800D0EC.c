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
    u8 padA, effectActive, animated, padD, frameTimer, frame;
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
static inline void setFrame(struct Sprite *sprite, u16 frame)
{
    sprite->frame = frame;
}
#define FallingObject_UpdateFalling sub_0800D0EC
void FallingObject_UpdateFalling(struct Object *object)
{
    if (object->entered) {
        object->entered = 0;
        if (object->animated)
            sub_08220C8C(object->anim, gUnk_0200046C->animation, 3, 0, 0);
    }
    object->position.y -= object->speed;
    if (object->position.y <= object->ground) {
        object->position.y = object->ground;
        if (object->type == 2) {
            if (object->flags & 4)
                sub_0822B2F8(669);
            else
                sub_0822B2F8(511);
        } else
            sub_0822B2F8(326);
        {
            u32 one = 1;
            u32 zero = 0;
            u32 frame;
            object->state = one;
            frame = 0;
            object->timer = zero;
            object->entered = one;
            object->frame = frame;
            object->frameTimer = frame;
        }
    }
    if (object->type > 1) {
        object->frameTimer++;
        if (object->frameTimer == 5) {
            object->frame++;
            if (object->frame > 2)
                object->frame = 0;
            object->frameTimer = 0;
        }
        setFrame(&object->sprite, object->frame);
    }
    if (object->collider)
        sub_0821FF24(object->collider, &object->position, 0);
    if (object->attack) {
        sub_0821FF24(object->attack, &object->position, 0);
        sub_0821FE40(object->attack);
    }
}
