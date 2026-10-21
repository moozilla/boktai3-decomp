// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
void Slime_UpdateStill(void);
void sub_08090724(void *);
extern const s16 gUnk_086149C4[];
struct SlimeDrawable {
    u32 flags;
    u8 filler[4];
    u8 a;
    s8 b;
};
struct SlimeAnim {
    u8 filler[8];
    u16 mode;
    u8 filler2[4];
    u16 frame;
};
struct SlimeVelocity {
    s16 x, y, z;
};
static inline s32 Shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
#define Slime_UpdateLaunch sub_0808F03C
void Slime_UpdateLaunch(struct SceneActorRegistryResetView *actor)
{
    struct SlimeDrawable *d = actor->drawable;
    u32 mask;
    struct SlimeAnim *anim = (struct SlimeAnim *)((u8 *)d + 0x48);
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SlimeVelocity *v;
        u32 heading;
        s32 speed;
        u8 *hp;
        actor->field2C2 = 0x20;
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, 0x40);
        hp = (u8 *)actor + 0xE9;
        v = (struct SlimeVelocity *)((u8 *)actor + 0x2EC);
        heading = *hp;
        speed = 0x12;
        asm("" : "+r"(speed));
        v->x = Shift12(gUnk_086149C4[(heading + 0x40) & 0xFF] * speed);
        v->y = 0;
        v->z = Shift12(gUnk_086149C4[heading] * speed);
        *(s16 *)((u8 *)actor + 0x2EE) = 0x28;
        d->flags |= 2;
        d->a = 0x40;
        d->b = 0x40;
        Sound_PlaySong(0x3C1);
        *(u16 *)((u8 *)actor + 0x2E8) = 0;
    }
    if (anim->mode == 3)
        anim->frame = 0;
    if (*(s16 *)((u8 *)actor + 0x2EE) > 0) {
        if (d->b <= 0x7E)
            d->b = d->b + 1;
    } else {
        if (d->b > 0x40)
            d->b = d->b - 2;
        if (d->b <= 0x3F)
            d->b = 0x40;
    }
    mask = 0x10;
    if (actor->flags2D0 & mask) {
        Chase_SetCtl(actor, Slime_UpdateStill, 0x2C);
        sub_08090724(actor);
        sub_08090724(actor);
        Sound_PlaySong(0x3C2);
    }
}
