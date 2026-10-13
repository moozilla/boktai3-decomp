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
void sub_08220C8C(void *, void *, u32, u32, u32);
void sub_0822B2F8(u32);
void sub_0821FF24(void *, void *, u32);
void sub_0821FE40(void *);
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
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
s32 AnimatedParticles_EmitBurst(void *, u32, u32, u32, u32, u32, u32, u32);
s32 FallingObject_SpawnFinishParticles(struct Object *object)
{
    struct PackedVec position;
    position.x = object->sprite.position.x;
    position.y = object->sprite.position.y;
    position.z = object->sprite.position.z;
    return AnimatedParticles_EmitBurst(&position, 2, 0, 64, 8, 4, 0, 0);
}
