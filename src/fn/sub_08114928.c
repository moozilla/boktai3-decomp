#include "global.h"
// confidence=low: profile-driven native effect motion and creation.
#ifdef NONMATCHING
// fuzzy: 90.6%
// fix.py best (sweep after the inlined-insn fix)
/* Open: the ROM masks the second group id with 0xFF (word load of the timer) where this
 * truncates, and the case-2 offset/constant temporaries swap r3/r4.
 * The u8/u16 form keeps the timer address from being shared with the first GroupId. */
#include "actor_attachment_state.h"
#include "ram.h"
struct Vec {
    s32 x, y, z, w;
};
struct Packed {
    s16 x, y, z, pad;
};
struct Camera {
    u8 pad[0x44];
    s32 x;
};
struct Effect {
    void (*update)(struct Effect *);
    u8 collider[0x5c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    void *data;
    struct Vec position, velocity;
    s32 height, speed;
    u16 lifetime;
    u8 angle, state;
    u16 id;
    u8 entered, active;
    s32 direction;
    u32 timer;
};

struct Manager {
    u8 task[0x18];
    struct Effect effects[16];
    u32 timer;
};
struct Profile {
    u8 unknown, type;
    u16 lifetime, unknown4, damage, parameter, threshold;
};
struct Manager *NativeEffect_EnsureManager(void);
u8 EffectPool_CheckCapacity(struct Manager *, s32, s32, s32);
void NativeEffect_InitTypeZeroSprite(struct Effect *);
void NativeEffect_InitDirectionalSprite(struct Effect *, s32);
void NativeEffect_InitThresholdSprite(struct Effect *, s32);
void NativeEffect_InitDirectionalTypeThreeSprite(struct Effect *, s32);
void NativeEffect_InitUnsequencedSprite(struct Effect *, s32);
void NativeEffect_InitDefaultCollider(struct Effect *, void (*)(void), s32);
void NativeEffect_InitColliderWithDepth(struct Effect *, void (*)(void), s32, s32);
void NativeEffect_OnColliderExpire(void);
void NativeEffect_OnColliderImpact(void);
void sub_0811455C(void);
void NativeEffect_UpdateLinearFlight(struct Effect *);
void NativeEffect_UpdateRotatingFlight(struct Effect *);
void NativeEffect_UpdateTurningTrail(struct Effect *);
void NativeEffect_SetState(struct Effect *, void (*)(struct Effect *));
void EffectActor_BeginHorizontalMotion(struct Effect *, struct Profile *, s32);
void Sound_PlaySong(u32);
static inline u32 GroupId(struct Manager *m, s32 group)
{
    u32 frame = m->timer;
    u32 mask = 255;
    return (group & mask) | ((frame & mask) << 8);
}
// confidence=low: selects a sprite and motion state from an emission profile.
s32 NativeEffect_CreateFromProfile(struct Vec *position, s32 height, struct Vec *velocity,
                                   struct Profile *profile, s32 group, s32 reverse, s32 required,
                                   s32 allowGroups)
{
    struct Manager *m;
    s32 i;
    if (!gSceneCallbackPool) {
        m = NativeEffect_EnsureManager();
        if (!m)
            return -1;
    } else
        m = gSceneCallbackPool;
    if (EffectPool_CheckCapacity(m, GroupId(m, group), required, allowGroups))
        return -1;
    for (i = 0; i < 16; i++) {
        if (!m->effects[i].active) {
            switch (profile->type) {
                case 0:
                    NativeEffect_InitTypeZeroSprite(&m->effects[i]);
                    break;

                case 1:
                    NativeEffect_InitDirectionalSprite(&m->effects[i], reverse);
                    break;

                case 2:
                    NativeEffect_InitThresholdSprite(&m->effects[i], profile->threshold);
                    break;

                case 3:
                    NativeEffect_InitDirectionalTypeThreeSprite(&m->effects[i], reverse);
                    break;

                case 4:
                    NativeEffect_InitUnsequencedSprite(&m->effects[i], reverse);
                    break;
            }

            m->effects[i].position = *position;
            m->effects[i].velocity = *velocity;
            m->effects[i].height = height;
            m->effects[i].lifetime = profile->lifetime;
            m->effects[i].sprite.flags |= 0x11;
            m->effects[i].sprite.priority = 1;
            m->effects[i].active = 1;
            m->effects[i].state = 0;
            {
                typeof(m->timer) v1 = m->timer;
                m->effects[i].id = ((u8)group) | ((u16)(v1 << 8));
            }
            switch (profile->type) {
                case 0:
                    m->effects[i].position.x += 0x4000;
                    m->effects[i].velocity.x += 0x4000;
                    m->effects[i].speed = m->effects[i].velocity.x >> 12;
                    NativeEffect_InitDefaultCollider(&m->effects[i], NativeEffect_OnColliderExpire,
                                                     profile->damage);
                    NativeEffect_SetState(&m->effects[i], NativeEffect_UpdateLinearFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 2:
                    m->effects[i].position.x += 0xc000;
                    m->effects[i].velocity.x += 0x5000;
                    m->effects[i].speed = m->effects[i].velocity.x >> 12;
                    if (profile->threshold > 60) {
                        s32 damage = profile->damage * 2;
                        NativeEffect_InitDefaultCollider(&m->effects[i], sub_0811455C, damage);
                    } else {
                        s32 damage = profile->damage;
                        NativeEffect_InitDefaultCollider(&m->effects[i],
                                                         NativeEffect_OnColliderExpire, damage);
                    }
                    NativeEffect_SetState(&m->effects[i], NativeEffect_UpdateLinearFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 1:
                    m->effects[i].position.x += 0x4000;
                    m->effects[i].velocity.x += 0x800;
                    m->effects[i].speed = m->effects[i].velocity.x;
                    NativeEffect_InitDefaultCollider(&m->effects[i], NativeEffect_OnColliderExpire,
                                                     profile->damage);
                    NativeEffect_SetState(&m->effects[i], NativeEffect_UpdateRotatingFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 3:
                    m->effects[i].position.x += 0x10000;
                    m->effects[i].velocity.x += 0x2000;
                    m->effects[i].speed = m->effects[i].velocity.x;
                    NativeEffect_InitColliderWithDepth(
                        &m->effects[i], NativeEffect_OnColliderImpact, profile->damage, 10);
                    NativeEffect_SetState(&m->effects[i], NativeEffect_UpdateTurningTrail);
                    Sound_PlaySong(0x164);
                    break;

                case 4:
                    EffectActor_BeginHorizontalMotion(&m->effects[i], profile, reverse);
                    Sound_PlaySong(0x164);
                    break;
            }

            return i;
        }
    }

    return -1;
}

#else
INCLUDE_ASM("asm/nonmatching", NativeEffect_CreateFromProfile);
#endif
