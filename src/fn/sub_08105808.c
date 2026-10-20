#include "global.h"
struct Collider {
    u8 pad0[0x34];
    u32 flags, reaction;
    u16 pad3C, damage, response;
    u8 pad42, contact;
    u8 pad44[8];
    void *callback, *data;
};
struct Actor {
    u8 pad0[0xA0];
    s16 position[4];
    struct Collider collider;
    s16 health, maximum;
    s32 damageStep;
    u32 unknown104, flags;
    u8 state, cooldown, unknown10E[3], variant;
};
void AnimatedParticles_EmitChargeSparkle(s32, s32, s32, s32, void *);
void Sound_PlaySong(u32);
void sub_08249244(void *, void *, void *, void *);
static inline u32 Mask(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u8 Test(struct Actor *p, u32 mask)
{
    if (p->flags & mask)
        return 1;
    return 0;
}
// confidence=low: collider callback applies damage and emits contact sparkles for both variants.
#define ProximityActor_ApplyContactDamage sub_08105808
void ProximityActor_ApplyContactDamage(struct Collider *a, struct Collider *b, struct Actor *p)
{
    u32 blocked;
    if (Test(p, 8))
        return;
    if (p->variant == 1) {
        blocked = Test(p, 2);
    } else {
        if (Mask(&a->flags, 16))
            goto ignore;
        blocked = Test(p, 2);
    }
    if (blocked)
        goto ignore;
    goto proceed;
ignore:
    b->damage = 0;
    b->response = 0;
    return;
proceed:
    if (Mask(&a->reaction, 8)) {
        b->contact = 1;
        if (p->variant == 0) {
            if (p->cooldown)
                p->cooldown--;
            else {
                AnimatedParticles_EmitChargeSparkle(2, 0, 0, 64, p->position);
                p->cooldown = 3;
                Sound_PlaySong(348);
            }
        } else if (p->variant == 1) {
            if (p->cooldown)
                p->cooldown--;
            else {
                AnimatedParticles_EmitChargeSparkle(2, 2, 0, 64, p->position);
                p->cooldown = 2;
                Sound_PlaySong(348);
            }
        }
        if (Test(p, 4)) {
            if (p->variant == 1) {
                p->health -= 200;
                p->health = p->health < 0 ? 0 : p->health;
            } else {
                p->health -= a->damage;
                p->health = p->health < 0 ? 0 : p->health;
            }
        }
    }
    if (p->variant == 1) {
        void *data = a->data;
        if (data) {
            void *callback = a->callback;
            if (callback && (Mask(&a->flags, 16)))
                sub_08249244(a, b, data, callback);
        }
    }
    return;
}
