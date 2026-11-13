#include "global.h"
#include "large_fragment_effect.h"
#include "ram.h"
#include "scene_steering_actor.h"
#include "engine.h"
#include "heap.h"
#include "resource_registry.h"
#include "monster_particle_random.h"
#include "scene_indicator.h"
#include "player_cue_sequence.h"
#include "scene_vec.h"

u32 LargeFragmentEffect_IsActive(void)
{
    u8 *s = gFragmentEffectManager;
    s32 i;
    u8 *p;
    if (s == NULL)
        return 0;
    i = 0;
    p = s + 0xf6;
    for (; i <= 7; p += 0xc0, i++) {
        if ((u8)(*p - 1) <= 1)
            return 1;
    }
    return 0;
}

void LargeFragmentEffect_StopAll(void)
{
    u8 *s = gFragmentEffectManager;
    s32 i;
    if (s) {
        u8 v = 3;
        s += 0xf6;
        for (i = 7; i >= 0; s += 0xc0, i--) {
            if (*s != 3 && *s != 0)
                *s = v;
        }
    }
}

void LargeFragmentEffect_OnCollision(void)
{
}

struct H {
    u32 lo : 16;
    u32 hi : 16;
};

struct Tail {
    u32 lo : 16;
    u32 : 16;
};

struct Z {
    struct H a;
    struct Tail b;
};

void LargeFragmentEffect_OnCollision(void); // a `bx lr` stub

void LargeFragmentEffect_InitCollider(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    struct Z s1, s2;
    u8 *q;
    s1.a.lo = 0x40;
    s1.a.hi = 0x80;
    s1.b.lo = 0x40;
    s2.a.lo = 0;
    s2.a.hi = 0;
    s2.b.lo = 0;
    q = p + 0x3C;
    Collider_Init(q, 0, 0x2401, 0, 0x10, &s1, &s2);
    Collider_SetAttackProperties(q, a, b, 0, c, d);
    Collider_SetCallback(q, LargeFragmentEffect_OnCollision, p);
    Collider_SetPosition(q, p + 0x1c, 0);
}

void LargeFragmentEffect_UpdateCollider(u8 *p)
{
    u8 *q = p + 0x3C;
    Collider_SetPosition(q, p + 0x1c, 0);
    Collider_Refresh(q);
}

static inline u8 SceneSlot_IsOccupied(struct LargeFragmentSlotPool *pool, u32 index)
{
    if ((1 << index) & pool->occupied)
        return 1;
    return 0;
}

struct LargeFragmentSlot *SceneSlot_Allocate(struct LargeFragmentSlotPool *pool)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!SceneSlot_IsOccupied(pool, i)) {
            pool->occupied |= 1 << i;
            {
                struct LargeFragmentSlot *slot = &pool->slots[i];
                slot->index = i;
                return slot;
            }
        }
    }
    return 0;
}

extern const s16 gUnk_086149C4[];

void SceneMovingParticle_Create(struct ContactPosition *, struct ContactPosition *, s32, s32);

static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void SceneSlot_EmitPulse(struct ContactPosition *position)
{
    struct ContactPosition velocity;
    struct ContactPosition *planar;
    u32 randomOrigin;
    u32 mask, sampleOrigin;
    s32 heading;
    s32 speed, vertical;

    /* The recovered bank origin cancels when converting a sample address to its table offset. */
    randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    mask = 255;
    heading = ParticleRandom_ReadDirectSample(gRandomTable) & mask;
    {
        s32 raw = ParticleRandom_ReadDirectSample(gRandomTable);
        s32 speedMask = 63;
        raw &= speedMask;
        planar = &velocity;
        speed = raw - 32;
    }
    {
        s32 x, zero, rawX;
        rawX = gUnk_086149C4[(heading + 64) & mask] * speed;
        sampleOrigin = randomOrigin;
        x = Truncate(rawX);
        zero = 0;
        planar->x = x;
        planar->y = zero;
        planar->z = Truncate(gUnk_086149C4[heading] * speed);
    }
    vertical = ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR);
    vertical &= 15;
    vertical += 30;
    velocity.y = vertical;
    SceneMovingParticle_Create(position, &velocity, 30, 3);
}

void SpriteEffectController_Create(void *, const void *, s32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, s32, s32, s32, s32, s32, s32);

static inline s32 FragmentTruncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

// Tentative: emits the small particles used by the large fragment animation.

void LargeFragmentEffect_SpawnRandomParticle(struct Vec *position)
{
    struct Vec velocity;
    struct Vec *planar;
    u32 randomOrigin;
    u32 mask, sampleOrigin;
    s32 heading;
    s32 speed;
    s32 lifetime;

    /* The recovered bank origin cancels when converting a sample address to its table offset. */
    randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    mask = 255;
    heading = ParticleRandom_ReadDirectSample(gRandomTable) & mask;
    {
        s32 raw = ParticleRandom_ReadDirectSample(gRandomTable);
        s32 speedMask = 15;
        raw &= speedMask;
        planar = &velocity;
        speed = raw + 4;
    }
    {
        s32 x, rawX;
        rawX = gUnk_086149C4[(heading + 64) & mask] * speed;
        sampleOrigin = randomOrigin;
        x = FragmentTruncate(rawX);
        planar->x = x;
        planar->y = 0;
        planar->z = FragmentTruncate(gUnk_086149C4[heading] * speed);
    }
    {
        const u16 *physical = (const u16 *)RANDOM_TABLE_ADDR;
        velocity.y = (ParticleRandom_ReadRelativeSample(sampleOrigin, physical) & 31) + 12;
        lifetime = (ParticleRandom_ReadRelativeSample(sampleOrigin, physical) & 63) + 20;
    }
    SpriteEffectController_Create(position, &velocity, 0x1C1A, 0xD1B8, 3, 0, 0x31, 0, 2, 0, 0,
                                  lifetime, 4, 1, 0x200004, 30, 8, 8);
}

/* LargeFragmentEffect_RunIdleHook: Empty function within the adjacent larger-collider fragment effect family */

void LargeFragmentEffect_RunIdleHook(void)
{
}

s32 Mod(s32, s32);

void SceneSlot_UpdateTimedPulse(void *unused, struct LargeFragmentSlot *slot)
{
    if (slot->reset) {
        slot->timer = 0;
        slot->reset = 0;
    }
    if (Mod(slot->timer, 6) == 0)
        SceneSlot_EmitPulse(slot->pulsePosition);
    if (slot->timer > slot->duration) {
        slot->state = 2;
        slot->reset = 1;
    } else
        slot->timer++;
}

struct FeatherSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};

struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct Vec position;
    u8 unknown24[8];
    struct FeatherSequence sequence;
    u8 unknown3C[0x54];
    u16 age;
    u8 unknown92[4];
    u8 state;
    s8 slot;
    u8 unknown98[4];
    u16 timer;
    u8 unknown9e, enter;
};

struct FeatherPool {
    u8 task[0x18];
    u8 graphics[0x1c];
    void *sequences;
    u32 used;
};

void LargeFragmentEffect_UpdateAnimation(void *unused, struct Feather *p)
{
    if (p->enter) {
        p->flags &= ~1;
        p->timer = 0;
        Sound_PlaySong(0x145);
        p->enter = 0;
    }
    if ((u16)(p->sequence.index - 1) <= 2) {
        LargeFragmentEffect_UpdateCollider((u8 *)p);
        if (Mod(p->timer, 6) == 0)
            LargeFragmentEffect_SpawnRandomParticle(&p->position);
    }
    p->timer++;
    if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)p,
                                    (struct DisplaySequenceState *)&p->sequence)) {
        p->state = 3;
        p->enter = 1;
    }
}

void Sprite_Remove(void *);

void SceneSlot_Remove(struct LargeFragmentSlotPool *pool, struct LargeFragmentSlot *slot)
{
    if (slot->reset) {
        slot->timer = 0;
        Sprite_Remove(slot);
        pool->occupied &= ~(1 << slot->index);
        slot->index = -1;
        slot->state = 0;
        slot->reset = 0;
    }
}

#define ScriptedColliderActors_EnsureManager FragmentSpritePool_GetOrCreate

#define ScriptedColliderActor_InitCollider LargeFragmentEffect_InitCollider

struct ScriptedColliderActor {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u8 unknown10[0xc];
    struct SceneVec position;
    u8 unknown24[8];
    u8 animation[0x10];
    u8 collider[0x54];
    void *script;
    u16 timer;
    u8 state, unknown97;
    s16 variant;
    u8 unknown9a[5], entered;
};

struct ScriptedColliderManager {
    u8 task[0x18];
    u8 resource[0x1c];
};

u8 *FragmentSpritePool_GetOrCreate(void);

void ScriptedColliderActor_Create(struct SceneVec *position, u32 variant, u32 a, u32 b, u32 c,
                                  u32 d)
{
    struct ScriptedColliderManager *manager = gScriptedColliderManager;
    struct ScriptedColliderActor *actor;
    if (!manager) {
        manager = (struct ScriptedColliderManager *)FragmentSpritePool_GetOrCreate();
        if (!manager)
            return;
    }
    actor =
        (struct ScriptedColliderActor *)SceneSlot_Allocate((struct LargeFragmentSlotPool *)manager);
    if (!actor)
        return;
    actor->variant = variant;
    if (actor->variant == 0)
        actor->state = 2;
    else
        actor->state = 1;
    actor->timer = 0;
    actor->entered = 1;
    actor->position = *position;
    actor->script = ScriptProc_Start(0x922e, 0x121b);
    Sprite_Init(actor, manager->resource, 0);
    Anim_SetSequence(actor->animation, actor->script, 6, 0, 0);
    AnimData_SelectPaletteBank(actor->graphics, 0x127);
    actor->priority = 2;
    actor->flags |= 1;
    ((void (*)())LargeFragmentEffect_InitCollider)(actor, a, b, c, d, 0, 0, 0);
}

extern void (*const gUnk_086107F8[])(void);

static inline u8 test(u32 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}

u32 FragmentSpritePool_UpdateTask(u8 *s)
{
    u8 *first = s + 0x3C;
    if (*(u32 *)(s + 0x38)) {
        s32 i = 0;
        void (*const *table)(void) = gUnk_086107F8;
        u8 *kind = s + 0xD2;
        u8 *entry = first;
        do {
            if (test((u32 *)(s + 0x38), 1 << i))
                sub_08249240(s, entry, table[*kind]);
            kind += 0xA0;
            entry += 0xA0;
            ++i;
        } while (i <= 7);
    }
    return 0;
}

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E {
    u8 a[0xA0];
};

struct FragmentDestroyManager {
    u8 pad[0x38];
    u32 mask;
    struct E e[8];
};

u32 FragmentSpritePool_DestroyTask(struct FragmentDestroyManager *b)
{
    s32 i = 0;
    struct E *e = b->e;
    for (; i < 8; e++, i++) {
        if (Tst(&b->mask, i))
            Sprite_Remove(e);
    }
    {
        u32 z = 0;
        gScriptedColliderManager = z;
    }
    return 0;
}

struct FragmentInitManager {
    u8 filler[0x18];
    u8 a[0x20];
    u32 c;
};

s32 FragmentSpritePool_Init(struct FragmentInitManager *s)
{
    s->c = 0;
    AnimData_Load(s->a, 0x0000F422);
    gScriptedColliderManager = s;
    return 0;
}

u8 *FragmentSpritePool_GetOrCreate(void)
{
    u8 *p;
    if (gScriptedColliderManager != 0)
        return gScriptedColliderManager;
    p = Task_Create(0xa, 0x53c);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))FragmentSpritePool_UpdateTask,
                          (void (*)(void *))FragmentSpritePool_DestroyTask);
        if (FragmentSpritePool_Init((struct FragmentInitManager *)p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void FragmentSpritePool_ResetSingleton(void)
{
    gScriptedColliderManager = 0;
}
