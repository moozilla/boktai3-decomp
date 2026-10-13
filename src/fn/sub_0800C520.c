#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
struct World {
    u8 pad[0x5a0];
    void *scene;
};
extern struct World *gUnk_02000710;
extern u16 gUnk_08604AB4[];
void sub_08013440(void *);
void sub_08013424(void *);
void sub_08215284(void *, u32);
static inline s32 isVisible(void *scene)
{
    void *saved = scene;
    s32 result;
    if (gUnk_02000710->scene == saved) {
        result = 1;
        asm("" : : "r"(result));
    } else {
        result = 0;
    }
    return result;
}
static inline void setFrame(struct Sprite *sprite, u16 frame)
{
    sprite->frame = frame;
}
s32 EffectEmitter_Update(struct Object *object)
{
    s32 visible = isVisible(object->scene);
    if (!visible) {
        object->sprite.flags |= 1;
        object->collision[3] |= 4;
        sub_08013440(object->shadow);
    } else {
        object->sprite.flags &= ~1;
        object->collision[3] &= ~4;
        sub_08013424(object->shadow);
        if (object->cooldown)
            object->cooldown--;
        if (object->flash) {
            object->flash--;
            if (!object->flash)
                sub_08215284(object->resource, 486);
        }
        if (++object->timer >= gUnk_08604AB4[object->phase]) {
            object->timer = 0;
            object->phase++;
            if (object->phase > 3)
                object->phase = 0;
            setFrame(&object->sprite, object->phase);
        }

        if (object->rising) {
            object->offset += 2;
            if (object->offset > 50) {
                object->offset = 50;
                object->rising = 0;
            }
        } else {
            object->offset -= 2;
            if (object->offset < -50) {
                object->offset = -50;
                object->rising = 1;
            }
        }
        object->sprite.position.y = object->baseY + object->offset;
    }
    return 0;
}
