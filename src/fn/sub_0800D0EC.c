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
void Anim_SetSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
static inline void setFrame(struct Sprite *sprite, u16 frame)
{
    sprite->frame = frame;
}
void FallingObject_UpdateFalling(struct Object *object)
{
    if (object->entered) {
        object->entered = 0;
        if (object->animated)
            Anim_SetSequence(object->anim, gUnk_0200046C->animation, 3, 0, 0);
    }
    object->position.y -= object->speed;
    if (object->position.y <= object->ground) {
        object->position.y = object->ground;
        if (object->type == 2) {
            if (object->flags & 4)
                Sound_PlaySong(669);
            else
                Sound_PlaySong(511);
        } else
            Sound_PlaySong(326);
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
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
}
