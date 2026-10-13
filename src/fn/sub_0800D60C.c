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
    u8 animationDone, effectActive, animated, remove, frameTimer, frame;
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
void sub_0800CC34(void *);
void sub_08015F60(u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
static inline u32 randomValue(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(u16 *)(offset + (u32)gUnk_0203B400);
}
static inline void setState(struct Object *object, u32 state)
{
    u32 value = state;
    u32 zero = 0;
    object->state = value;
    object->timer = zero;
    object->entered = 1;
}
void FallingObject_SpawnFinishParticles(void *);
void FallingObject_UpdateFinish(struct Object *object)
{
    if (object->entered) {
        object->entered = 0;
        Anim_SetSequence(object->anim, gUnk_0200046C->animation, 4, 0, 0);
        if (object->collider) {
            u32 flags = ((u16 *)object->collider)[3];
            u32 result;
            result = 4;
            result |= flags;
            ((u16 *)object->collider)[3] = result;
        }
        sub_0800CC34(object);
    }
    if (object->animationDone) {
        if (!object->remove)
            FallingObject_SpawnFinishParticles(object);
        object->remove = 1;
    }
}
