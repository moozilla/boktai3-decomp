#include "global.h"
#include "engine_globals.h"

struct PackedVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct ShortVector {
    s16 x, y, z, pad;
};
struct DamageTransform {
    u8 unk_00[0xc];
    struct ShortVector coordinates;
};
struct HresvelgrDamage {
    u8 unk_00[0x48];
    struct DamageTransform transform;
    u8 unk_5C[0x34];
    u32 timer;
    s16 health;
    u8 unk_96[6];
    u32 flags;
    u8 unk_A0;
    u8 controllerFlags;
    u8 stateReset;
    u8 unk_A3;
    u32 pending;
    u8 unk_A8[3];
    u8 state;
    u8 unk_AC[5];
    u8 changed;
    u8 unk_B2[0xa];
    u32 contactSource;
    u8 unk_C0[0x40];
    void (*callback)(void *);
    u8 unk_104[0xb0];
    struct ShortVector renderPosition;
    u8 unk_1BC[0x12];
    u16 paletteId;
    u8 unk_1D0[0xc];
    void *palette;
    u8 unk_1E0[0x68];
    u8 damageDisplay[0xc8];
    u16 debrisTimer;
    u16 flashTimer;
    u16 timer314;
    u16 unk_316;
    u16 timer318;
    u16 environmentTimer;
    u16 primaryDamage;
    u16 secondaryDamage;
    u16 primaryFlags;
    u16 secondaryFlags;
    u16 reactionDamage;
    u16 debris;
    u8 sparks;
    u8 reaction;
    u8 unk_32A[0x4a];
    s16 reactionThreshold;
    u8 unk_376[0x18e];
    u8 hitPalette[1];
};
struct EnvironmentState {
    u8 unk_00[0x876];
    s16 level;
};
extern struct EnvironmentState *gGameState;
extern u16 gUnk_0203B400[];
s32 Hresvelgr_GetEnvironmentalLevel(void *);
void Hresvelgr_EmitRandomMotionBurst(void *);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void MotionEntry_CreateWithDelta(void *, void *, s32, s32);
void Sound_PlaySong(s32);
void Sound_PlaySongChecked(s32);

static inline s32 nextRandom(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)table);
}
static inline void spawnSparks(struct HresvelgrDamage *p)
{
    struct PackedVector size;
    s32 angle, count, speed;
    const u16 *table;
    size.x = 256;
    size.y = 256;
    size.z = 256;
    table = gUnk_0203B400;
    angle = (u8)nextRandom(table);
    count = (nextRandom(table) & 3) + 2;
    speed = (nextRandom(table) & 3) + 20;
    SparkParticle_Spawn(&p->renderPosition, 3, &size, angle, count, speed, 2, 2);
}
static inline void setReaction(struct HresvelgrDamage *p, void (*callback)(void *), s32 state)
{
    int zero;
    p->state = state;
    zero = 0;
    p->callback = callback;
    p->timer = zero;
    p->changed = 1;
    p->stateReset = zero;
    p->pending = zero;
}
static inline void setScale(struct ShortVector *v, s16 x, s16 y, s16 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
static inline void addFlags(struct HresvelgrDamage *p, u32 flags)
{
    p->flags |= flags;
}
static inline void setHitPalette(struct HresvelgrDamage *p, s32 id)
{
    p->paletteId = id;
    p->palette = p->hitPalette;
}
static inline u32 hasControllerFlag(struct HresvelgrDamage *p, u32 mask)
{
    return mask & p->controllerFlags;
}
static inline void emitDebris(void *emitter)
{
    int i;

    for (i = 0; i < 2; i++) {
        struct PackedVector delta = {0};
        const u16 *table = gUnk_0203B400;
        delta.x = (nextRandom(table) & 15) - 8;
        delta.y = nextRandom(table) & 7;
        delta.z = (nextRandom(table) & 15) - 8;
        MotionEntry_CreateWithDelta(emitter, &delta, nextRandom(table) % 4, 60);
        /* Keep the table lookup local to each entry. */
        table = 0;
    }
}
// Tentative: consumes contact hits and selects Hresvelgr damage reactions.
s32 Hresvelgr_ApplyContactDamage(struct HresvelgrDamage *p)
{
    struct HresvelgrDamage *saved = p;
    s32 sparks;
    s32 reaction;
    s32 debris;
    s32 damage;
    s32 flags;
    s32 sound;
    struct DamageTransform *transform = &p->transform;
    void *display = p->damageDisplay;
    sparks = p->sparks;
    reaction = p->reaction;
    debris = p->debris;
    p->sparks = 0;
    p->reaction = 0;
    p->debris = 0;
    if (p->primaryDamage) {
        damage = p->primaryDamage;
        flags = p->primaryFlags;
    } else {
        damage = p->secondaryDamage;
        flags = p->secondaryFlags;
    }
    saved->primaryDamage = 0;
    saved->secondaryDamage = 0;
    saved->primaryFlags = 0;
    saved->secondaryFlags = 0;
    if (flags & 0x40) {
        sparks = 0;
        reaction = 0;
        debris = 0;
        sound = 0x211;
    } else {
        sound = 0x13e;
    }
    if (!saved->environmentTimer) {
        if (!saved->timer314 && !saved->timer318 && Hresvelgr_GetEnvironmentalLevel(saved)) {
            damage = gGameState->level * 5;
            spawnSparks(saved);
            Sound_PlaySongChecked(0x12a);
            saved->environmentTimer = 60;
        }
    } else {
        saved->environmentTimer--;
    }
    if (damage <= 0 && !(flags & 0x40))
        goto noReaction;
    if (hasControllerFlag(p, 2) && p->contactSource == 16) {
        addFlags(p, 0x04000000);
        goto reacted;
    }
    p->health -= damage;
    saved->flashTimer = 10;
    setHitPalette(saved, 0x2f7);
    {
        struct ShortVector scale;
        setScale(&scale, 16, 0, 16);
        DamageNumberDisplay_Show(display, damage, 0, &transform->coordinates, &scale, 1);
    }
    if (p->health <= 0) {
        p->health = 0;
        addFlags(p, 2);
        goto noReaction;
    }
    if (p->state == 16) {
        if (p->contactSource & 16) {
            if ((saved->reactionDamage += damage) > 180) {
                saved->reactionDamage = 0;
                addFlags(p, 0x80000000);
                goto reacted;
            }
            goto noReaction;
        }
        if (p->contactSource & 1) {
            saved->reactionDamage = 0;
            setReaction(p, Hresvelgr_EmitRandomMotionBurst, 33);
            goto reacted;
        }
    }
    if (p->state == 33) {
        if (++saved->reactionDamage > 3) {
            saved->reactionDamage = 0;
            addFlags(p, 0x10000000);
            goto reacted;
        }
        setReaction(p, Hresvelgr_EmitRandomMotionBurst, 33);
        goto reacted;
    }
    saved->reactionDamage += damage;
    if (saved->reactionDamage > saved->reactionThreshold) {
        reaction = 1;
        saved->reactionDamage = 0;
    }
    if (sparks)
        spawnSparks(saved);
    if (debris) {
        saved->debrisTimer = 15;
        emitDebris(&saved->renderPosition);
    }
    if (reaction) {
        addFlags(p, 1);
    } else {
        goto playSound;
    }
reacted:
    return 1;
playSound:
    Sound_PlaySong(sound);
noReaction:
    return 0;
}
