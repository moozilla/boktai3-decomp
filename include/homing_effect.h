#ifndef HOMING_EFFECT_H
#define HOMING_EFFECT_H
#include "global.h"

// Tentative: a pool of six particle effects (each four sprites) that drift around, home in on the
// player actor and apply an effect to it when they arrive. Owned by a task (struct EffectManager).

struct EffectVec {
    s16 x, y, z, pad;
};

struct EffectPackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct EffectSprite {
    u32 flags; // bit 2/3: flip
    u8 pad4[4];
    u16 tile;
    u8 width, height;
    u8 pad0C[12];
    struct EffectVec position;
    u8 tail[8];
};

struct EffectAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};

// Particle state (index into gUnk_08604A78[type]): updated by the EffectParticle_Update* functions.
struct EffectParticle {
    u8 state, speed, angle;
    s8 drift;
    u16 timer, lifetime;
    struct EffectVec velocity, origin;
    struct EffectAnim anim;
    struct EffectSprite sprite;
};

struct Effect {
    u8 active, count, type, kind, soundDelay, pad;
    u16 timer;
    struct EffectParticle particles[4];
};

struct EffectGraphics {
    u32 pad;
    u16 base;
};

// The player actor (partial view).
struct EffectActor {
    u8 pad0[32];
    u32 flags;
    u8 pad24[12];
    struct EffectVec position;
    u8 pad38[0x36c];
    u8 facing;
    u8 pad3A5[0x73];
    u8 mode; // 6: alternate effect mode
};

struct EffectManager {
    u8 task[24];
    u32 mode, counter;
    struct EffectGraphics *graphics;
    void *animation;
    struct Effect effects[6];
    struct EffectActor *actor;
};

// Screen-space boxes used for the arrival test.
struct EffectContext {
    struct EffectVec world, lower, upper, actorLower, actorUpper;
};

struct EffectAnimId {
    u16 id, pad;
};

typedef s32 (*EffectParticleUpdate)(struct EffectManager *, struct Effect *, s32, u32, u32,
                                    struct EffectContext *);

extern struct EffectManager *gUnk_02000040;
extern struct EffectActor *gUnk_02000580;
extern s16 gUnk_086149C4[];
extern u32 gUnk_02000560;
extern u32 gUnk_030053F4, gUnk_030053F0;
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern struct EffectAnimId gUnk_08604A30[][3];
extern u32 gUnk_08604A60[];
extern u32 gUnk_08604A6C[];
extern void (*gUnk_08604A54[])(void *, u32);
extern EffectParticleUpdate gUnk_08604A78[][5];

void sub_0800A800(void);
u32 sub_0800A80C(struct EffectManager *, struct Effect *);
s32 sub_0800A830(void *);
u32 sub_0800A86C(void *);
s32 EffectManager_UpdateParticles(struct EffectManager *, struct Effect *, struct EffectContext *);
s32 EffectManager_UpdateAll(struct EffectManager *);
s32 sub_0800AB18(struct EffectManager *);
s32 EffectManager_Init(struct EffectManager *, void *);
void *sub_0800AC00(void *);

/* Engine helpers. */
void sub_0822B3BC(u32);
void sub_08220C8C(void *, void *, u32, u32, u32);
s32 sub_082215E4(s32, s32);
void sub_08217EAC(void *);
void sub_08217E60(void *, void *, u32);
void sub_08217ECC(void *, u32);
void *sub_08215184(u32);
void *sub_0821A520(u32, u32);
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);

#endif
