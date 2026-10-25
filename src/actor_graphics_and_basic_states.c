#include "global.h"
#include "actor_graphics_and_basic_states.h"
#include "contact_actor.h"

int SceneActorGraphics_GetResourceId(struct StateActor *p)
{
    switch (p->locked) {
        case 0:
            return 0x335;
        case 1:
            return 0x335;
        default:
            return 0x335;
    }
}

u8 *SceneActorGraphics_GetDefaultResourceId(void)
{
    return (u8 *)0x00000335;
}

int SceneActorGraphics_GetResourceId(struct StateActor *);

// MATCHING_CONCESSION: register-pinning
void SceneActorGraphics_SelectResource(u8 *p)
{
    /* Raw offsets, not struct SceneActorGraphics fields (0x38c effectScale,
     * 0x38e effectMode, 0x390 resourceId, 0x288 palette, 0x3c4/0x5c4/0x7c4
     * palette banks): going through the struct perturbs this pinned match. */
    if (p[0x38e]) {
        int resource;
        p[0x38c] = 0;
        p[0x38e] = 0;
        *(int *)(p + 0x390) = SceneActorGraphics_GetResourceId((struct StateActor *)p);
        resource = SceneActorGraphics_GetResourceId((struct StateActor *)p);
        {
            int test = resource;
            asm("" : "+r"(test));
            if (test == 0)
                *(void **)(p + 0x288) = p + 0x5c4;
            else if (test == 1)
                *(void **)(p + 0x288) = p + 0x7c4;
            else {
                {
                    void *src = gUnk_030042E4 + resource * 32;
                    register void *dst asm("r1") = p + 0x3c4;
                    CpuSet(src, dst, 0x4000008);
                }
                *(void **)(p + 0x288) = p + 0x3c4;
            }
        }
    }
}

#define SceneActorGraphics_OnDamageContact sub_08021F4C
extern u8 *gUnk_02000580;
extern u32 gUnk_030053F4;
void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void Sound_PlaySongChecked(u32);
void DamageNumberDisplay_Show(void *, u32, u32, void *, void *, u32);
s32 Math_Atan2(s32, s32);
s32 WirelessGame_TryAddCyclingItem(void *);
void WirelessGame_AddItem(void *, s32);
struct ContactVector {
    s16 x, y, z;
};
struct DamageContactActor {
    u8 pad[0x33c];
    union {
        u32 value;
        u16 halves[2];
    } damage;
};

static inline u32 TestContactFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}

void SceneActorGraphics_OnDamageContact(u8 *source, u8 *contact, u8 *p)
{
    u8 *actor = p;
    u8 *sprite = p + 0x48;
    u32 mode;
    struct ContactVector size;
    u8 *position;
    s32 item;
    u16 responseValue;
    if (*((u16 *)(p + 0x34e)))
        return;
    mode = gUnk_02000580[0x418];
    Collision_CalculateDamageResponse(source, contact);
    if (((*((u32 *)(p + 0x358))) == 1) && (mode != 2))
        *((u16 *)(contact + 0x3e)) = 0;
    if (!(gUnk_030053F4 & 0x1000))
        PlayerStats_AddContactHundredths(source, contact, *((s16 *)(p + 0x380)));
    ((struct DamageContactActor *)p)->damage.value = *((u16 *)(contact + 0x3e));
    if ((*((u16 *)(contact + 0x3e))) == 0) {
        Sound_PlaySongChecked(0x211);
        *((u16 *)(p + 0x34c)) = 10;
        *((void **)(p + 0x288)) = p + 0x5c4;
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(contact, *((u16 *)(contact + 0x3e)), 0, sprite + 12, &size, 1);
    }
    {
        u32 k1 = 0x100;
        if ((*((u16 *)(source + 6))) & k1) {
            u8 angle = source[0x42];
            p[0x342] = angle;
        } else
            p[0x342] = Math_Atan2((*((s16 *)(contact + 0x24))) - (*((s16 *)(source + 0x24))),
                                  (*((s16 *)(contact + 0x28))) - (*((s16 *)(source + 0x28))));
    }
    {
        u32 value = *((u32 *)(source + 0x34));
        *((u32 *)(actor + 0xbc)) = value;
        value = *((u32 *)(source + 0x38));
        *((u32 *)(actor + 0xc0)) = value;
    }
    responseValue = *((u16 *)(source + 0x44));
    *((u16 *)(p + 0x34e)) = responseValue;
    PlayerStats_AddContactHundredths(source, contact, actor[0xe2]);
    if (((*((s32 *)(p + 0x33c))) > 0) && TestContactFlags((u32 *)(source + 0x38), 1))
        p[0x340] = 1;
    if (!(gUnk_030053F4 & 0x1000))
        return;
    if ((mode - 1) <= 1) {
        position = contact + 0xc;
        if (WirelessGame_TryAddCyclingItem(position) && (contact[0x43] & 8))
            WirelessGame_AddItem(position, 0);
        return;
    }
    if (contact[0x43] & 8)
        WirelessGame_AddItem(contact + 0xc, 0);
    if (!TestContactFlags((u32 *)(source + 0x38), 0x800000))
        return;
    {
        u8 *stats = (u8 *)gUnk_02000710;
        u32 offset = *((s16 *)(stats + 0x52)) << 1;
        item = (s16)(*((u16 *)(offset + (u32)stats + 0x88)) - 6);
    }
    switch ((u32)item) {
        case 0:
            WirelessGame_AddItem(contact + 0xc, 2);
            break;

        case 1:
            WirelessGame_AddItem(contact + 0xc, 1);
            break;

        case 2:
            WirelessGame_AddItem(contact + 0xc, 3);
            break;

        case 3:
            contact += 0xc;
            WirelessGame_AddItem(contact, 2);
            WirelessGame_AddItem(contact, 1);
            break;

        case 4:
            contact += 0xc;
            WirelessGame_AddItem(contact, 2);
            WirelessGame_AddItem(contact, 3);
            break;

        case 5:
            contact += 0xc;
            WirelessGame_AddItem(contact, 2);
            WirelessGame_AddItem(contact, 1);
            WirelessGame_AddItem(contact, 3);
            break;
    }
}

void SceneActorGraphics_InitBodyNoOp(void)
{
}

typedef struct {
    s16 x, y, z;
} Vec;

void SceneActorGraphics_InitBodyNoOp(void);

extern u32 gUnk_030053F4;

void SceneActorGraphics_InitializeBody(struct StateActor *p)
{
    Vec scale, rotation;
    void *drawable = p->bodyDrawable;
    scale.x = 0x40;
    scale.y = 0x80;
    scale.z = 0x40;
    rotation.x = 0;
    rotation.y = 0x80;
    rotation.z = 0;
    Collider_Init(drawable, 0, 0x2001, 0, 16, &scale, &rotation);
    Collider_SetPosition(drawable, &p->worldPosition, 0);
    Collider_SetCallback(drawable, SceneActorGraphics_InitBodyNoOp, p);
    {
        int test = gUnk_030053F4 & 0x1000;
        int flags;
        flags = 0;
        if (test)
            flags = 0x40800;
        /* Not through struct SceneActorGraphics: that CSEs the groundEffect base and
         * reads the three fields as small offsets off it, unlike the original. */
        Collider_SetAttackProperties(drawable, *(s16 *)((u8 *)p + 0x35c), *(s16 *)((u8 *)p + 0x35e),
                                     0, flags, *(s16 *)((u8 *)p + 0x360));
    }
}

void sub_08021F4C(u8 *, u8 *, u8 *);

void SceneActorGraphics_InitializeOverlay(struct StateActor *p, int resource)
{
    Vec scale, rotation;
    void *drawable = p->overlayDrawable;
    scale.x = 0x60;
    scale.y = 0x80;
    scale.z = 0x60;
    rotation.x = 0;
    rotation.y = 0x80;
    rotation.z = 0;
    Collider_Init(drawable, 0, 0x5101, 0, 16, &scale, &rotation);
    Collider_SetPosition(drawable, &p->worldPosition, 0);
    Collider_SetCallback(drawable, sub_08021F4C, p);
    Collider_SetBodyProperties(drawable, resource, 0x802, 1);
    Collider_Refresh(drawable);
}

/* Raw offsets throughout: routing any of this through struct StateActor /
 * SceneActorGraphics changes register allocation enough to break the match
 * (tried: `p->actionFlags`, `p->bodyDrawable`, struct-field attack properties). */

void SceneActorGraphics_UpdateVariant(struct StateActor *p)
{
    u8 *drawable = (u8 *)p + 0x178;
    {
        u32 mask = 0x40000;
        if (*(u32 *)((u8 *)p + 0x9c) & mask)
            ((u8 *)p)[0x3b6] = 1;
        else
            ((u8 *)p)[0x3b6] = 0;
    }
    if (((u8 *)p)[0x3b6] != ((u8 *)p)[0x3b7]) {
        switch (((u8 *)p)[0x3b6]) {
            case 0:
                Collider_SetAttackProperties(drawable, *(s16 *)((u8 *)p + 0x35c),
                                             *(s16 *)((u8 *)p + 0x35e), 0, 0,
                                             *(s16 *)((u8 *)p + 0x360));
                break;
            case 1:
                Collider_SetAttackProperties(drawable, *(s16 *)((u8 *)p + 0x362),
                                             *(s16 *)((u8 *)p + 0x364), 0, 0x60000,
                                             *(s16 *)((u8 *)p + 0x366));
                break;
        }
        {
            int value = ((u8 *)p)[0x3b6];
            ((u8 *)p)[0x3b7] = value;
        }
    }
}

/* A flat `pad[0x344]` prefix (matching groundEffect's offset) rather than
 * struct StateActor + a nested SceneActorGraphics cast: the two-step access
 * changes register allocation enough to break the match. Fields are
 * SceneActorGraphics's scale/scaleMin/scaleMax/scaleStep (0x344-0x34B). */

typedef struct {
    u8 pad[0x344];
    s16 scale, min, max, step;
} Actor;

void SceneActorGraphics_OscillateScale(Actor *p)
{
    p->scale += p->step;
    if (p->step > 0) {
        if (p->scale >= p->max) {
            p->scale = p->max;
            p->step = -32;
        }
    } else {
        if (p->scale <= p->min) {
            p->scale = p->min;
            p->step = 32;
        }
    }
}

extern u16 gUnk_02000590;

s32 SceneActorGraphics_GetEffectState(struct StateActor *p)
{
    s32 r;
    if (gUnk_02000590 != 0) {
        r = 4;
    } else if (AreaEffect_IsPositionCovered(&p->target) == 0) {
        r = 0;
    } else {
        r = *(s16 *)((u8 *)gUnk_02000710 + 0x876);
    }
    return r;
}

void SceneActorState_SetValueClampedWhenInactive(struct StateActor *p, u32 v)
{
    if (p->variant == 0)
        v = 3;
    p->actionAnimation = v;
}

void SceneActorState_SetValueClampedWhenActive(struct StateActor *p, u32 v)
{
    if (p->variant == 1)
        v = 3;
    p->actionAnimation = v;
}

typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct {
        u32 x, y;
    } pos;
    u8 pad5c[0x4c];
    u8 facing, facing8, padAA[2], mode, state, phase, finished, enter;
    u8 padB1[0x13];
    u32 elapsed;
    u8 padC8[4];
    u8 requestedAngle;
    u8 padCD[0x2df];
    u32 graphicsMode;
} ActorAnimation;

static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);

// MATCHING_CONCESSION: register-pinning
void SceneActors_StartDisappearEffect(u32 *position)
{
    ActorAnimation *p = SceneActors_GetBySlot(0);
    if (p) {
        int sequence;
        u32 one;
        u32 zero = 0, phase = 13;
        p->mode = zero;
        p->state = zero;
        p->phase = phase;
        p->elapsed = zero;
        {
            u8 *dst = &p->enter;
            one = 1;
            *dst = one;
        }
        p->pos = *(typeof(p->pos) *)position;
        setFacing(p, p->requestedAngle);
        {
            int test = p->graphicsMode & one;
            sequence = 62;
            if (test)
                sequence = 24;
        }
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, one,
                                              one, 6);
        Sound_PlaySongChecked(839);
        Sound_PlaySongChecked(1335);
    }
}

void ContactActorPool_RequestClear(struct ContactActorPool *);

void SceneActors_RequestContactPoolClear(void)
{
    struct StateActor *p = SceneActors_GetBySlot(0);
    if (p && ((struct SceneActorGraphics *)p->groundEffect)->contactPool != 0)
        ContactActorPool_RequestClear(((struct SceneActorGraphics *)p->groundEffect)->contactPool);
}

void SceneActors_RequestContactPoolClear(void);

void SceneActors_AddHealthCost(int amount)
{
    struct StateActor *p;
    if (amount > 0) {
        p = SceneActors_GetBySlot(0);
        if (p) {
            int remaining, diff;
            /* Raw offsets into groundEffect (healthCostAccumulator 0x3A0,
             * healthCostBaseline 0x3A4): a shared SceneActorGraphics pointer
             * changes register allocation enough to break the match. */
            *(s32 *)((u8 *)p + 0x3a0) += amount;
            remaining = p->health - *(s32 *)((u8 *)p + 0x3a0);
            if (remaining <= 1)
                goto stop;
            {
                diff = remaining - *(s32 *)((u8 *)p + 0x3a4);
                if (diff < 0)
                    diff = -diff;
                if (diff > 49) {
                stop:
                    SceneActors_RequestContactPoolClear();
                }
            }
        }
    }
}

void SceneActors_AddResourceCost(int amount)
{
    struct StateActor *p;
    if (amount > 0) {
        p = SceneActors_GetBySlot(0);
        if (p)
            *(int *)((u8 *)p + 0x39c) += amount; /* resourceCostAccumulator */
    }
}

// confidence=low: index of the first of the count tracked positions at 0x110 that lies inside
// the box min..max on x and z, or -1.
struct BoxSearchPosition {
    s16 x, y, z;
};

static inline s32 ReadPositionReference(u32 *entry, struct BoxSearchPosition **out)
{
    *out = (struct BoxSearchPosition *)*entry;
    return *out != NULL;
}

// MATCHING_CONCESSION: register-pinning
// Finds the first tracked position inside the inclusive x/z bounds.
s32 sub_08022624(void *actor, s32 count, struct BoxSearchPosition *min,
                 struct BoxSearchPosition *max)
{
    u8 *bytes = actor;
    s32 i;

    i = 0;
    if (i >= count)
        goto end;
loop: {
    s32 offset = i * 4;
    struct BoxSearchPosition *pos;
    u32 address = (u32)(bytes + 0x110);

    address += offset;
    ReadPositionReference((u32 *)address, &pos);
    if (pos) {
        s32 x = pos->x;
        s32 minX = min->x;
        register s32 inside asm("r4") = 0;

        if (x >= minX && x <= max->x && pos->z >= min->z && pos->z <= max->z)
            inside = 1;
        if (inside)
            return i;
    }
}
    i++;
    if (i < count)
        goto loop;
end:
    return -1;
}

typedef struct ParticleOffset {
    s16 x, y, z, pad;
} ParticleOffset;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 SceneParticle_CreateParticles(const void *anchor, const ParticleOffset *offset,
                                  const void *range, const void *jitter, s32 count, u32 priority);

// confidence=low: emits four particles from the anchor at p + 0x54 around a facing-dependent
// center, each offset by a random -64..63 in x and z.
void sub_08022694(u8 *p)
{
    u8 *base = p + 0x48;
    ParticleOffset center;
    ParticleOffset offset;
    s32 i;
    if (p[0xa8] <= 1) {
        center.x = 0x40;
        center.y = 0xc8;
        center.z = -0x80;
    } else {
        center.x = -0x80;
        center.y = 0xc8;
        center.z = 0x40;
    }
    for (i = 0; i < 4; i++) {
        const u16 *table = gUnk_0203B400;
        u16 index = (gUnk_03005308 + 1) & 0x3FF;
        {
            u16 random = table[index];
            offset.x = ((random & 0x7F) + center.x) - 0x40;
        }
        offset.y = center.y;
        index = (index + 1) & 0x3FF;
        gUnk_03005308 = index;
        {
            u16 random = table[index];
            offset.z = ((random & 0x7F) + center.z) - 0x40;
        }
        table = 0;
        SceneParticle_CreateParticles(base + 0xC, &offset, table, table, 1, 1);
    }
}

struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

extern u32 gUnk_03005308;

// confidence=low: five sparks 300 units above the actor; the large burst for big, otherwise a
// smaller one shifted by the facing kept at 0xA8.
void sub_08022790(u8 *p, s32 big)
{
    struct ContactPosition pos;
    struct Packed size;
    struct ContactPosition *at = &pos;
    s32 angle, count, speed;
    u16 index;
    u32 *indexPtr;
    u32 mask;
    int smallMask;
    const u16 *table;
    pos = *(struct ContactPosition *)(p + 0x260);
    at->y += 300;
    if (big) {
        size.x = 164;
        size.y = 32;
        size.z = 164;
    } else {
        switch (p[0xA8]) {
            case 1:
                pos.x -= 0x80;
                break;
            case 2:
                at->z -= 0x80;
                break;
        }
        size.x = 128;
        size.y = 16;
        size.z = 128;
    }
    table = gUnk_0203B400;
    indexPtr = &gUnk_03005308;
    index = *indexPtr + 1;
    mask = 1023;
    index &= mask;
    angle = (u8)table[index];
    index = (index + 1) & mask;
    count = table[index];
    smallMask = 3;
    count = (count & smallMask) + 2;
    index = (index + 1) & mask;
    *indexPtr = index;
    speed = (table[index] & smallMask) + 20;
    SparkParticle_Spawn(&pos, 5, &size, angle, count, speed, 0, 2);
}

typedef struct BurstVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
} BurstVec;
extern const BurstVec gUnk_08604D84[];
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
u16 Sqrt(u32);
s32 Div(s32, s32);
void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
void TransientSpriteEffects_Spawn(int, int, void *, void *, int, int);

#ifdef NONMATCHING
// fuzzy: 30.8%
// scope shape 95 (differences once low registers are set aside)
// confidence=low: bursts eight particles around fixed offsets from p, each flying outward
// from a point 0xc8 above p at speed 5.
void sub_08022878(BurstVec *p)
{
    BurstVec center = *p;
    s32 i;
    BurstVec pos[1], vel[1];
    center.y += 0xc8;
    for (i = 0; i < 8; i++) {
        const u16 *table = gUnk_0203B400;
        u32 index;
        u16 len;
        u16 r;
        index = (gUnk_03005308 + 1) & 0x3FF;
        gUnk_03005308 = index;
        r = table[index] & 0x1f;
        pos->x = gUnk_08604D84[i].x + p->x - 0x10 + r;
        index = (index + 1) & 0x3FF;
        gUnk_03005308 = index;
        r = table[index] & 0x1f;
        pos->y = gUnk_08604D84[i].y + p->y - 0x10 + r;
        index = (index + 1) & 0x3FF;
        gUnk_03005308 = index;
        r = table[index] & 0x1f;
        pos->z = gUnk_08604D84[i].z + p->z - 0x10 + r;
        AnimatedParticles_EmitBurst(pos, 1, 0, 0x40, 0xc, 0xc, 4, 4);
        vel->x = pos->x - center.x;
        vel->y = pos->y - center.y;
        vel->z = pos->z - center.z;
        len = Sqrt(vel->x * vel->x + vel->y * vel->y + vel->z * vel->z);
        if (len) {
            vel->x = Div(vel->x * 5, len);
            vel->y = Div(vel->y * 5, len);
            vel->z = Div(vel->z * 5, len);
        }
        TransientSpriteEffects_Spawn(2, 0, pos, vel, 0x20, 0x100);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08022878);
#endif

// MATCHING_CONCESSION: register-pinning
void SceneActors_SpawnAmbientParticlePair(struct StateActor *p)
{
    u8 *graphics = (u8 *)p + 0x240;
    struct Packed size;
    int i;
    for (i = 0; i < 2; i++) {
        int angle, speed;
        int count;
        register u32 index asm("r1");
        u32 *indexPtr;
        u32 mask;
        int smallMask;
        register const u16 *table asm("r4");
        size.x = 164;
        size.y = 16;
        size.z = 164;
        indexPtr = &gUnk_03005308;
        mask = 1023;
        smallMask = 3;
        table = gUnk_0203B400;
        index = (*indexPtr + 1) & mask;
        angle = (u8)table[index];
        index = (index + 1) & mask;
        count = (table[index] & smallMask) + 2;
        index = (index + 1) & mask;
        *indexPtr = index;
        speed = (table[index] & smallMask) + 20;
        SparkParticle_Spawn((struct ContactPosition *)(graphics + 0x20), 3, &size, angle, count,
                            speed, 2, 2);
    }
}

void SceneActors_StopBackgroundFlash(struct StateActor *p)
{
    /* Raw offsets (backgroundFlash 0x9DC, backgroundFlashFlag 0x3C0): a
     * shared SceneActorGraphics pointer breaks the match. */
    BackgroundFlash_BeginDisappearance(*(struct BackgroundFlashController **)((u8 *)p + 0x9dc));
    *(u32 *)((u8 *)p + 0x3c0) = 0;
}

struct ReactionActor {
    u8 pad0[0x90];
    u32 timer;
    u8 pad94[8];
    u32 flags;
    u8 padA0[2];
    u8 reset;
    u8 padA3;
    u32 pending;
    u8 facing, facing8, requested, state;
    u8 active, actionReset, action, finished, entered, changed;
    u8 padB2[0x12];
    u32 actionTimer;
    u8 padC8[0x38];
    void (*callback)(void *);
};
void SceneActors_UpdateShrinkingReaction(void *);
void SceneActors_UpdateKnockbackReaction(void *);
void sub_08027214(void *);
void WirelessGame_AddItem(void *, s32);

static inline void reactionAction(struct ReactionActor *p, int one, int id, int zero)
{
    p->active = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = one;
}

static inline void reactionState(struct ReactionActor *p, void (*fn)(void *), int id)
{
    int zero;
    p->state = id;
    zero = 0;
    p->callback = fn;
    p->timer = zero;
    p->changed = 1;
    p->reset = zero;
    p->pending = zero;
}

// confidence=low: damage handler: 0 shows the damage number and starts the shrinking reaction
// (wireless games may award an item when hit from the front), 2 starts a knockback, 1 a death.
s32 sub_08022B18(struct ReactionActor *p, s32 kind, s32 damage)
{
    u8 *actor = (u8 *)p;
    u8 *sprite = (u8 *)p + 0x48;
    s32 result = 1;
    switch (kind) {
        case 0: {
            s32 shown = damage;
            if (*(u32 *)((u8 *)p + 0x358) == 1 && gUnk_02000580[0x418] != 2) {
                shown = 0;
                result = 0;
            }
            if (damage > 0) {
                struct ContactVector size;
                size.x = 0x20;
                size.y = 0;
                size.z = 0x20;
                DamageNumberDisplay_Show(actor + 0x1cc, shown, 0, sprite + 0xc, &size, 1);
            }
            {
                u32 mask = 0x100000;
                if (p->flags & mask)
                    SceneActors_StopBackgroundFlash((struct StateActor *)actor);
            }
            reactionAction(p, 1, 0, 0);
            reactionState(p, SceneActors_UpdateShrinkingReaction, 0x16);
            if (gUnk_030053F4 & 0x1000) {
                u8 *position = sprite + 0xc;
                if (WirelessGame_TryAddCyclingItem(position)) {
                    s32 d =
                        actor[0x20e] -
                        Math_Atan2(*(s16 *)(sprite + 0xc) - *(s16 *)((u8 *)gUnk_02000710 + 0x30),
                                   *(s16 *)(sprite + 0x10) - *(s16 *)((u8 *)gUnk_02000710 + 0x34));
                    if (d < 0)
                        d = -d;
                    if (d <= 0x20)
                        WirelessGame_AddItem(position, 0);
                }
            }
            break;
        }
        case 2:
            reactionAction(p, 1, 1, 0);
            reactionState(p, SceneActors_UpdateKnockbackReaction, 0x17);
            {
                u8 v = sprite[9] + 0x80;
                ((u8 *)p)[0x342] = v;
            }
            break;
        case 1: {
            s32 zero;
            {
                u8 v = (*(u8 **)((u8 *)p + 0x10c))[0x2d];
                u8 *dst = (u8 *)p + 0x342;
                zero = 0;
                *dst = v;
            }
            {
                u32 mask = 2;
                p->flags |= mask;
            }
            reactionAction(p, 1, 3, zero);
            reactionState(p, sub_08027214, 0x19);
            break;
        }
    }
    return result;
}

void SceneActorState_StepTableByte(struct StateActor *p, const u8 *tbl)
{
    /* Raw offsets into groundEffect's step-table fields (row 0x3B0, col 0x3B1,
     * value 0x3B2, previous value 0x3B4). */
    u16 *cur = (u16 *)((u8 *)p + 0x3b2);
    u16 old = *cur;
    u8 *col;
    u32 row;
    *(u16 *)((u8 *)p + 0x3b4) = old;
    row = ((u8 *)p)[0x3b0];
    col = (u8 *)p + 0x3b1;
    {
        u32 idx = row * 8 + (u32)tbl;
        idx += *col;
        *cur = *(u8 *)idx;
    }
    (*col)++;
    if (*col > 7)
        *col = 0;
}

extern const u8 gUnk_08604DC4[], gUnk_08604DE4[], gUnk_08604E04[], gUnk_08604E24[];

void SceneActorState_StepTableByte(struct StateActor *p, const u8 *tbl);

int SceneActorState_StepTableByKind(struct StateActor *p)
{
    const u8 *tbl;
    switch (*(s32 *)((u8 *)p + 0x358)) { /* stepTableKind */
        case 0:
            tbl = gUnk_08604DC4;
            break;
        case 1:
            tbl = gUnk_08604DE4;
            break;
        case 2:
            tbl = gUnk_08604E04;
            break;
        case 3:
            tbl = gUnk_08604E24;
            break;
        default:
            goto fail;
    }
    SceneActorState_StepTableByte(p, tbl);
    return 1;
fail:
    return 0;
}

// confidence=low: chooses the next action phase from the reaction kind and flash state.
#define SceneActorState_SelectReactionPhase sub_08022DBC
// MATCHING_CONCESSION: register-pinning
static inline s32 phaseValue(u8 *p)
{
    s32 value = *p;
    return value;
}
void SceneActorState_SelectReactionPhase(u8 *p)
{
    u32 phase;
    u8 *actor = p;
    if (*(u32 *)(p + 0x358) == 3) {
        if (p[0x3b9]) {
            phase = 14;
            goto set;
        }
    } else {
        if (p[0x3b9] == 3)
            SceneActors_StopBackgroundFlash(p);
        else if (p[0x3b9]) {
            phase = 14;
            asm("" : : "i"(1));
            goto set;
        }
    }
    switch (*(u16 *)(actor + 0x3b2)) {
        case 0:
            phase = 9;
            break;
        case 1:
            phase = 10;
            break;
        case 2:
            if (*(u32 *)(actor + 0x358) != 3 && phaseValue(p + 0xe1) <= 63) {
                u8 *flag = actor + 0x394;
                *flag = 1;
            } else {
                u8 *flag = actor + 0x394;
                *flag = 0;
            }
            phase = 2;
            break;
        case 3:
            phase = 11;
            break;
        case 4:
            if (phaseValue(p + 0xe1) <= 63) {
                phase = 14;
                asm("" : : "i"(3));
            } else {
                phase = 11;
                asm("" : : "i"(4));
            }
            break;
        case 5:
            phase = 15;
            break;
        case 6:
            phase = 16;
            break;
        default:
            return;
    }
set: {
    register u8 *phaseField asm("r2") = p + 0xae;
    u32 zero = 0;
    *phaseField = phase;
    *(u32 *)(p + 0xc4) = zero;
    p[0xb0] = 1;
}
}

// confidence=low: reaction-kind selector controls the far-point mode and motion timer.
#define SceneActorState_SnapReactionPosition sub_08022EA4
// MATCHING_CONCESSION: register-pinning
void Path_SnapToRandomFarPoint(void *, void *, u32, u32);
struct SnapVector {
    u32 a, b;
};
void SceneActorState_SnapReactionPosition(u8 *p, struct SnapVector *position)
{
    u8 *sprite = p + 0x48;
    switch (*(u16 *)(p + 0x3b2)) {
        case 0:
            Path_SnapToRandomFarPoint(position, sprite + 12, 0, 65536);
            break;
        case 1:
            Path_SnapToRandomFarPoint(position, sprite + 12, 2, 65536);
            break;
        case 2:
            Path_SnapToRandomFarPoint(position, sprite + 12, 1, 65536);
            *(u16 *)(p + 0x354) = 32;
            return;
        case 3:
            Path_SnapToRandomFarPoint(position, sprite + 12, 3, 65536);
            break;
        case 4:
            Path_SnapToRandomFarPoint(position, sprite + 12, 3, 65536);
            asm("" : : "i"(4));
            break;
        case 5:
            Path_SnapToRandomFarPoint(position, sprite + 12, 3, 65536);
            asm("" : : "i"(5));
            break;
        case 6:
            Path_SnapToRandomFarPoint(position, sprite + 12, 2, 65536);
            asm("" : : "i"(6));
            break;
        default:
            *position = *(struct SnapVector *)(p + 0x54);
            break;
    }
    *(u16 *)(p + 0x354) = 64;
}

#ifdef NONMATCHING
// fuzzy: 71.3%

// fuzzy: 81.1%

// Scene-actor cutscene dispatcher (Vanargand, Ratatosk). Commands mostly set

// state 2 with a sub-state (setState); 2 places the actor, 9 configures an

// effect, 15/16 pick the body graphics variant, 25 flashes the background.

// Still differs: sb/r8 swapped for entry/i; in cases 15/16 the graphics

// variant branches use other registers, so their tails are not cross-jumped

// as in the original.

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

struct S {
    u8 pad0[0x54];
    u16 h54, h56, h58;
    u8 pad5a[0xAC - 0x5A];
    u8 state;
    u8 entered;
    u8 sub;
    u8 padAF;
    u8 changed;
    u8 padB1[0xC4 - 0xB1];
    u32 timer;
    u8 padC8[0xD0 - 0xC8];
    u16 hD0, hD2, hD4;
    u8 padD6[0x114 - 0xD6];
    u8 angle;
    u8 pad115[0x118 - 0x115];
    Entry entry;
};

struct T {
    u8 pad0[0x260];
    u16 h260, h262, h264;
    u8 pad266[0x288 - 0x266];
    u8 *palette;
    u8 pad28c[0x344 - 0x28C];
    u16 h344;
    u8 pad346[0x38C - 0x346];
    u8 b38C;
    u8 pad38d;
    u8 b38E;
    u8 b38F;
    u32 w390;
    u8 pad394[0x3B8 - 0x394];
    u8 b3B8;
    u8 pad3b9;
    u8 b3BA;
    u8 pad3bb[0x3C4 - 0x3BB];
    u8 pal3C4[0x5C4 - 0x3C4];
    u8 pal5C4[0x7C4 - 0x5C4];
    u8 pal7C4[0x9C4 - 0x7C4];
    u16 h9C4, h9C6, h9C8;
    u8 pad9ca[0x9DC - 0x9CA];
    void *flash;
};

extern u8 *gUnk_0200047C;

extern u8 *gUnk_030042E4;

void SceneMessages_SetPendingTrigger(void *, void *);

void CpuSet(const void *, void *, u32);

static inline void setStateFull(struct S *s, u32 state, u32 entered, u32 sub)
{
    s->state = state;
    s->entered = entered;
    s->sub = sub;
    s->timer = entered;
}

#define setState(s, sub) setStateFull(s, 2, 0, sub)

static inline void effect2(u8 *p, int mode, int id, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}

#define effect(p, id, mode, d) effect2(p, mode, id, d)

void SceneActors_DispatchCommands(struct S *s)
{
    struct T *t = (struct T *)s;
    Entry *entry = &s->entry;
    s32 i;

    i = 0;
    if (i < entry->count[gUnk_0200047C[0x24]])
        do {
            s16 *m;
            u8 *controller = gUnk_0200047C;
            m = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, m);
            switch (((u8 *)m)[6]) {
                case 0:
                    setState(s, 0);
                    s->changed = 1;
                    break;
                case 1:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 1);
                    s->changed = 1;
                    break;
                case 3:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 2);
                    s->changed = 1;
                    break;
                case 4:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 3);
                    s->changed = 1;
                    break;
                case 5:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 4);
                    s->changed = 1;
                    break;
                case 6:
                    s->angle = (m[4] + 5) << 5;
                    setState(s, 5);
                    s->changed = 1;
                    break;
                case 7:
                    setState(s, 6);
                    s->changed = 1;
                    break;
                case 8:
                    setState(s, 7);
                    s->changed = 1;
                    break;
                case 10:
                    setState(s, 8);
                    s->changed = 1;
                    break;
                case 11:
                    setState(s, 9);
                    s->changed = 1;
                    break;
                case 12:
                    t->h9C4 = m[4];
                    t->h9C6 = m[5];
                    t->h9C8 = m[6];
                    setState(s, 10);
                    s->changed = 1;
                    break;
                case 19:
                    setState(s, 11);
                    s->changed = 1;
                    break;
                case 20:
                    setState(s, 12);
                    s->changed = 1;
                    break;
                case 21:
                    setState(s, 13);
                    s->changed = 1;
                    break;
                case 23:
                    setState(s, 14);
                    s->changed = 1;
                    break;
                case 24:
                    setState(s, 15);
                    s->changed = 1;
                    break;
                case 25:
                    if (m[4])
                        SceneActors_StopBackgroundFlash(t);
                    else if (m[5])
                        BackgroundFlash_BeginAlternating(t->flash);
                    else
                        BackgroundFlash_BeginAppearance(t->flash);
                    break;
                case 13:
                    setState(s, 16);
                    s->changed = 1;
                    break;
                case 14:
                    setState(s, 18);
                    s->changed = 1;
                    break;
                case 22:
                    s->hD0 = m[4];
                    s->hD2 = m[5];
                    s->hD4 = m[6];
                    setState(s, 19);
                    s->changed = 1;
                    break;
                case 18:
                    setState(s, 20);
                    s->changed = 1;
                    break;
                case 15:
                    if (m[4] == 0) {
                        s32 r;
                        t->b38C = 0;
                        t->b38E = 0;
                        t->w390 = SceneActorGraphics_GetResourceId(t);
                        r = SceneActorGraphics_GetResourceId(t);
                        switch (r) {
                            case 0:
                                t->palette = t->pal5C4;
                                break;
                            case 1:
                                t->palette = t->pal7C4;
                                break;
                            default:
                                CpuSet(gUnk_030042E4 + r * 32, t->pal3C4, 0x04000008);
                                t->palette = t->pal3C4;
                                break;
                        }
                    } else if (m[5] == 0) {
                        effect((u8 *)t, 0x338, 3, 5);
                    } else {
                        effect((u8 *)t, 0x33A, 3, 5);
                    }
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 16:
                    if (m[4] == 0)
                        effect((u8 *)t, 0x335, 1, 5);
                    else if (m[5] == 0)
                        effect((u8 *)t, 0x338, 2, 5);
                    else
                        effect((u8 *)t, 0x33A, 2, 5);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 9:
                    SceneActors_ConfigureEffect(s, m[4], 0xB4, 0xB4);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 17:
                    t->b3B8 = m[4];
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 2:
                    s->h54 = m[4];
                    s->h56 = m[5];
                    s->h58 = m[6];
                    t->h260 = s->h54;
                    t->h262 = t->h344 + s->h56;
                    t->h264 = s->h58;
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 66:
                    t->b3BA = m[4];
                    setState(s, 21);
                    s->changed = 1;
                    break;
            }
            i++;
        } while (i < entry->count[gUnk_0200047C[0x24]]);
}

#else

INCLUDE_ASM("asm/nonmatching", SceneActors_DispatchCommands);

#endif

static inline u8 TakeFlag(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return TRUE;
    }
    return FALSE;
}

void SceneActorState_AdvanceToState1A(struct StateActor *p)
{
    if (TakeFlag(p)) {
        u32 v = 0x1a;
        p->actionAnimation = v;
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

static inline u8 chk_08023638(struct StateActor *p)
{
    u8 *q = &p->actionEnter;
    if (*q) {
        *q = 0;
        q--;
        *q = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState01(struct StateActor *p)
{
    u8 r = chk_08023638(p);
    if (r == 1) {
        p->actionAnimation = r;
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

void SceneActorState_AdvanceToState02(struct StateActor *p)
{
    if (TakeFlag(p)) {
        u32 v = 2;
        p->actionAnimation = v;
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

static inline u8 chk_080236C0(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_Acknowledge(struct StateActor *p)
{
    if (chk_080236C0(p)) {
        u32 k = 3;
        p->actionAnimation = k;
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

static inline u8 chk_08023720(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState1B(struct StateActor *p)
{
    if (chk_08023720(p)) {
        u32 k = 0x1b;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

static inline u8 chk_08023770(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState1C(struct StateActor *p)
{
    if (chk_08023770(p)) {
        u32 k = 0x1C;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

/* Raw offsets throughout the recovery block below (actionMode 0xAC,
 * actionIndex 0xAD, actionPhase 0xAE, actionTimer 0xC4, actionFinished 0xAF):
 * struct-field writes for that cluster pick a different (still correct, but
 * non-matching) register for the last store. */

struct S_080237C0 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_080237C0(struct S_080237C0 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_Resume(struct S_080237C0 *p)
{
    if (chk_080237C0(p)) {
        u32 k = 0x1d;
        p->aa = k;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    }
    if (p->af) {
        u32 two = 2;
        u32 z = 0;
        u32 one = 1;
        p->ac = two;
        p->ad = z;
        p->ae = one;
        p->c4 = z;
        p->b0 = one;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    } else
        p->c4++;
}

/* Raw offsets in the recovery block: see SceneActorState_Resume for why. */

struct S_0802383C {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_0802383C(struct S_0802383C *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState1E(struct S_0802383C *p)
{
    if (chk_0802383C(p)) {
        u32 k = 0x1e;
        p->aa = k;
    }
    if (p->af) {
        u32 two = 2;
        u32 z = 0;
        p->ac = two;
        p->ad = z;
        p->ae = z;
        p->c4 = z;
        p->b0 = 1;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    } else
        p->c4++;
}

static inline u8 chk_080238A8(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState23(struct StateActor *p)
{
    if (chk_080238A8(p)) {
        u32 k = 0x23;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

static inline u8 chk_080238F8(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState24(struct StateActor *p)
{
    if (chk_080238F8(p)) {
        u32 k = 0x24;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

/* Raw offsets: see SceneActorState_Resume for why the triplet write needs this shape. */

struct S_08023948 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1[4];
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 padC8[0x2cc];
    u8 particle;
};

static inline u8 chk_08023948(struct S_08023948 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState06(struct S_08023948 *arg)
{
    struct S_08023948 *p = arg;
    struct S_08023948 *base = p;
    if (chk_08023948(p)) {
        u32 k = 6;
        u8 *state = &p->aa;
        u32 zero;
        zero = 0;
        *state = k;
        base->particle = zero;
    }
    if (p->af)
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    else
        p->c4++;
}

static inline u8 chk_080239A8(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState20(struct StateActor *p)
{
    if (chk_080239A8(p)) {
        u32 k = 0x20;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

static inline u8 chk_080239F8(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_AdvanceToState21(struct StateActor *p)
{
    if (chk_080239F8(p)) {
        u32 k = 0x21;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

static inline u8 chk(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_Hold(struct StateActor *p)
{
    if (chk(p)) {
        u32 k = 0x22;
        p->actionAnimation = k;
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (!p->actionFinished)
        p->actionTimer++;
}

void SceneActorState_PlayAnimation17AndConsumeTrigger(struct StateActor *p)
{
    if (chk(p)) {
        u32 k = 0x11;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation19AndConsumeTrigger(struct StateActor *p)
{
    if (chk(p)) {
        u32 k = 0x13;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

/* Raw offsets: see SceneActorState_Resume. */

struct S_08023B38 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_08023B38(struct S_08023B38 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayAnimation14ThenAdvance(struct S_08023B38 *p)
{
    if (chk_08023B38(p)) {
        u32 k = 0xe;
        p->aa = k;
    }
    if (p->af) {
        u32 next = 0x11;
        u32 z;
        p->ae = next;
        z = 0;
        p->c4 = z;
        p->b0 = 1;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    } else
        p->c4++;
}

void SceneActorState_PlayAnimation15AndConsumeTrigger(struct StateActor *p)
{
    if (TakeFlag(p)) {
        u32 v = 0xf;
        p->actionAnimation = v;
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

/* Raw offsets: see SceneActorState_Resume. */

struct S_08023BE4 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_08023BE4(struct S_08023BE4 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_ResumeAfterAction(struct S_08023BE4 *p)
{
    if (chk_08023BE4(p)) {
        u32 k = 0x10;
        p->aa = k;
    }
    if (p->af) {
        u32 two = 2;
        u32 z = 0;
        u32 one = 1;
        p->ac = two;
        p->ad = z;
        p->ae = one;
        p->c4 = z;
        p->b0 = one;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    } else
        p->c4++;
}

void SceneActorState_PlayAnimation31AndConsumeTrigger(struct StateActor *p)
{
    if (chk(p)) {
        u32 k = 0x1F;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

/* Raw offsets: see SceneActorState_Resume. */

struct S_08023CA0 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_08023CA0(struct S_08023CA0 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayAnimation37ThenResume(struct S_08023CA0 *p)
{
    if (chk_08023CA0(p)) {
        u32 k = 0x25;
        p->aa = k;
    }
    if (p->af) {
        u32 two = 2;
        u32 z = 0;
        p->ac = two;
        p->ad = z;
        p->ae = z;
        p->c4 = z;
        p->b0 = 1;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    } else
        p->c4++;
}

void SceneActorState_PlayAnimation39AndConsumeTrigger(struct StateActor *p)
{
    if (chk(p)) {
        u32 k = 0x27;
        p->actionAnimation = k;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

extern const u32 gUnk_08604E44[];

void SceneActorState_DispatchPhaseHandler(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604E44[p->actionPhase]);
}

/* Raw offsets throughout: the state()/action() helpers each write several
 * adjacent fields together (callback+state+timer+changed+reset+pending, or
 * active+actionReset+action+actionTimer+entered); going through
 * struct StateActor's named fields for those clusters breaks the match
 * (see SceneActorState_Resume). */

struct Common {
    u8 pad0[0x90];
    u32 timer;
    s16 health, maxHealth;
    u8 pad98[2];
    u8 healthCheck, pad9b;
    u32 flags;
    u8 padA0[2];
    u8 reset;
    u8 padA3;
    u32 pending;
    u8 facing, facing8, requested, state;
    u8 active, actionReset, action, finished, entered, changed;
    u8 padB2[0x12];
    u32 actionTimer;
    u8 padC8[0x38];
    void (*callback)(void *);
};

static inline void state(struct Common *p, void (*fn)(void *), int id)
{
    int zero;
    p->state = id;
    zero = 0;
    p->callback = fn;
    p->timer = zero;
    p->changed = 1;
    p->reset = zero;
    p->pending = zero;
}

int GridAttack_SelectNextAction(void *);

void sub_08027214(void *);

static inline u8 chk_08023D78(struct Common *p)
{
    if (p->entered) {
        p->entered = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

static inline void action(struct Common *p, int one, int id, int zero)
{
    p->active = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = one;
}

int SceneActorState_TryStartDeathOrFlaggedAction(struct Common *p)
{
    if (p->health <= 0 && p->healthCheck) {
        u32 mask = 2;
        p->flags |= mask;
        state(p, sub_08027214, 25);
    }
    p->healthCheck = 0;
    {
        u32 mask = 2;
        if (p->flags & mask) {
            action(p, 1, 3, 0);
            return 1;
        } else if (p->flags & 1) {
            action(p, 1, 2, 0);
            return 1;
        }
    }
    return 0;
}

/* Raw offsets: the ending phase/elapsed/enter_08023E24 triplet write breaks the match
 * when routed through struct StateActor's named fields (see SceneActorState_Resume). */

typedef struct State_08023E24 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter_08023E24, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_08023E24;

typedef struct Actor_08023E24 {
    State_08023E24 base;
    u8 pad104[0x23f];
    u8 variant;
    u8 pad344[0x12];
    u16 delay;
} Actor_08023E24;

static inline u8 enter_08023E24(State_08023E24 *p)
{
    if (p->enter_08023E24) {
        p->enter_08023E24 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void sub_08022EA4(u8 *, struct SnapVector *);

void SceneActorState_WaitForDelayedAction(Actor_08023E24 *p)
{
    State_08023E24 *s = &p->base;
    s->vulnerable = 1;
    if (enter_08023E24(s) == 1) {
        if (p->variant == 0)
            s->animation = 1;
        else {
            u32 k = 2;
            s->animation = k;
        }
    }
    if (s->elapsed > p->delay && SceneActorState_StepTableByKind(p)) {
        sub_08022EA4(p, (u8 *)p + 0x9c4);
        {
            u32 one = 1;
            u32 zero;
            s->phase = one;
            zero = 0;
            s->elapsed = zero;
            s->enter_08023E24 = one;
        }
    } else
        s->elapsed++;
}

typedef struct State_08023EC0 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter_08023EC0, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_08023EC0;

typedef struct Actor_08023EC0 {
    State_08023EC0 base;
    u8 pad104[0x23f];
    u8 variant;
    u8 pad344[0x12];
    u16 delay;
} Actor_08023EC0;

static inline u8 enter_08023EC0(State_08023EC0 *p)
{
    if (p->enter_08023EC0) {
        p->enter_08023EC0 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void sub_08022DBC(u8 *);

void SceneActorState_UpdateVariantActionPlayback(Actor_08023EC0 *p)
{
    State_08023EC0 *s = &p->base;
    s->vulnerable = 1;
    if (enter_08023EC0(s))
        SceneActorState_SetValueClampedWhenInactive(p, 4);
    if (s->finished) {
        if (p->base.state != 3) {
            sub_08022DBC(p);
            p->delay = 60;
            return;
        } else {
            u32 k = 4;
            p->base.animation = k;
        }
    }
    s->elapsed++;
}

typedef struct State_08023F34 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter_08023F34, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_08023F34;

static inline u8 enter_08023F34(State_08023F34 *p)
{
    if (p->enter_08023F34) {
        p->enter_08023F34 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayAnimation5ThenAdvance(State_08023F34 *p)
{
    p->vulnerable = 1;
    if (enter_08023F34(p)) {
        u32 k = 5;
        p->animation = k;
    }
    if (p->finished) {
        u32 next = 3;
        u32 zero;
        p->phase = next;
        zero = 0;
        p->elapsed = zero;
        p->enter_08023F34 = 1;
    } else
        p->elapsed++;
}

typedef struct State_08023F98 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp;
    s16 maxHp;
    u16 extra;
    u8 vulnerable;
    u8 kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2;
    u8 unknownA3;
    u32 unknownA4;
    u8 facing;
    u8 facing8;
    u8 animation;
    u8 state;
    u8 mode;
    u8 submode;
    u8 phase;
    u8 finished;
    u8 enter_08023F98;
    u8 changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_08023F98;

typedef struct Actor_08023F98 {
    State_08023F98 base;
    u8 pad104[0x23f];
    u8 variant;
    u8 pad344[0x12];
    u16 delay;
} Actor_08023F98;

static inline u8 enter_08023F98(State_08023F98 *p)
{
    if (p->enter_08023F98) {
        p->enter_08023F98 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_EnterPhase6AndResetAnimation(State_08023F98 *p)
{
    int bit;
    {
        u32 next = 6;
        u32 zero;
        p->phase = next;
        zero = 0;
        p->elapsed = zero;
        p->enter_08023F98 = 1;
    }
    {
        u32 k = 9;
        p->animation = k;
    }
    (*((void (**)(void *))(((u8 *)p) + 0x108)))(p);
    p->animation = 0;
    {
        u32 *flags = &p->flags;
        u32 mask = ~0x40u;
        bit = 0x40;
        p->flags &= ~bit;
    }
    ((u8 *)p)[0x383] = 0;
}

struct ProbeBox {
    s32 x : 16, y : 16, z : 16, w : 16;
};

// confidence=low: state 6 of the actor's recovery pipeline: grab a tracked target in reach in
// front (variant 1), otherwise advance to phase 5 once the animation ends or the ground 0x190
// ahead is more than two tiles above or below.

// confidence=low: state 6 of the actor's recovery pipeline: grab a tracked target in reach in
// front (variant 1), otherwise advance to phase 5 once the animation ends or the ground 0x190
// ahead is more than two tiles above or below.

// confidence=low: state 6 of the actor's recovery pipeline: grab a tracked target in reach in
// front (variant 1), otherwise advance to phase 5 once the animation ends or the ground 0x190
// ahead is more than two tiles above or below.
struct ProbePos {
    s16 x, y, z, w;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 ProbeTruncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline void ProbeProject(struct ProbePos *out, u8 angle, s32 distance)
{
    out->x = ProbeTruncate12(gUnk_086149C4[(angle + 0x40) & 0xff] * distance);
    out->y = 0;
    out->z = ProbeTruncate12(gUnk_086149C4[angle] * distance);
}

static inline void ProbeAdd(struct ProbePos *dst, struct ProbePos *src)
{
    dst->x += src->x;
    dst->y += src->y;
    dst->z += src->z;
}

static inline s32 ProbeCellAt(struct ProbePos *p)
{
    s32 x = p->x >> 8;
    s32 z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

static inline u8 *ProbeCellProps(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + (cell * 4 + 12);
}

#ifdef NONMATCHING
// fuzzy: 89.6%
// scope shape 5 (differences once low registers are set aside)
// confidence=low: state 6 of the actor's recovery pipeline: grab a tracked target in reach in
// front (variant 1), otherwise advance to phase 5 once the animation ends or the ground 0x190
// ahead is more than two tiles above or below.
void sub_08023FE8(State_08023F98 *p)
{
    u8 *actor = (u8 *)p;
    struct ProbePos *sprite = (struct ProbePos *)((u8 *)p + 0x48);
    p->vulnerable = 1;
    if (enter_08023F98(p)) {
        u32 k = 6;
        p->animation = k;
    }
    if (actor[0x394] == 1) {
        struct ProbePos off, min, max;
        s32 r;
        ProbeProject(&off, ((u8 *)sprite)[9], 0x80);
        ProbeAdd(&off, (struct ProbePos *)((u8 *)sprite + 0xc));
        min.x = -0x80;
        min.z = -0x80;
        ProbeAdd(&min, &off);
        max.x = 0x80;
        max.z = 0x80;
        ProbeAdd(&max, &off);
        r = sub_08022624(actor, actor[0x174], (struct BoxSearchPosition *)&min,
                         (struct BoxSearchPosition *)&max);
        if (r >= 0) {
            u8 *q = (&gUnk_02000580)[r];
            u32 mask = 0x40000;
            if (!(*(u32 *)(q + 0x20) & mask)) {
                actor[0x395] = r;
                SceneActorState_EnterPhase6AndResetAnimation(p);
                return;
            }
        }
    }
    {
        struct ProbePos pos;
        s32 distance = 0x190;
        u8 *s = actor + 0x48;
        u8 *props;
        u32 type;
        s32 h;
        u32 bad;
        ProbeProject(&pos, s[9], distance);
        ProbeAdd(&pos, (struct ProbePos *)(s + 0xc));
        props = ProbeCellProps(ProbeCellAt(&pos));
        type = *props >> 4;
        h = (*props & 0xf) << 8;
        switch (type) {
            case 1:
                h -= (u8)pos.z;
                break;
            case 2:
                h -= (u8)pos.x;
                break;
        }
        {
            u32 d = h - *(s16 *)(actor + 0x352) + 0x1ff;
            bad = 0;
            if (d > 0x3fe)
                bad = 1;
        }
        if (bad || p->finished) {
            u32 next = 5;
            u32 zero;
            p->phase = next;
            zero = 0;
            p->elapsed = zero;
            p->enter_08023F98 = 1;
        } else
            p->elapsed++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08023FE8);
#endif

struct S_0802429C {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_0802429C(struct S_0802429C *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayAnimation7ThenRecover(struct S_0802429C *p)
{
    struct S_0802429C *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk_0802429C(p, z)) {
        u32 k = 7;
        p->aa = k;
    }
    if (p->af) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000)
            p2->h356 = z2;
        else
            p2->h356 = 0x5a;
    } else
        p->c4++;
}

struct S_08024334 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024334(struct S_08024334 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayAnimation8ThenRecover(struct S_08024334 *p)
{
    struct S_08024334 *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk_08024334(p, z)) {
        u32 k = 8;
        p->aa = k;
    }
    if (p->af) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000)
            p2->h356 = z2;
        else
            p2->h356 = 0x1E;
    } else
        p->c4++;
}

INCLUDE_ASM("asm/nonmatching", sub_080243CC);

struct S_08024440 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024440(struct S_08024440 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayAnimation10ThenRecover(struct S_08024440 *p)
{
    struct S_08024440 *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk_08024440(p, z)) {
        u32 k = 10;
        p->aa = k;
    }
    if (p->af) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000)
            p2->h356 = z2;
        else
            p2->h356 = 0x1E;
    } else
        p->c4++;
}

struct S_080244D8 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_080244D8(struct S_080244D8 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayAnimation11ThenRecover(struct S_080244D8 *p)
{
    struct S_080244D8 *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk_080244D8(p, z)) {
        u32 k = 11;
        p->aa = k;
    }
    if (p->af) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000)
            p2->h356 = z2;
        else
            p2->h356 = 0x1E;
    } else
        p->c4++;
}

struct S_08024570 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 ab;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024570(struct S_08024570 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayVariantAnimation12ThenRecover(struct S_08024570 *p)
{
    struct S_08024570 *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk_08024570(p, z))
        SceneActorState_SetValueClampedWhenActive(p2, 0xc);
    if (p->af) {
        if (p2->ab == 3) {
            u32 k = 0xc;
            p2->aa = k;
            goto inc;
        } else {
            u32 z2 = 0;
            p2->ac = z2;
            p2->ad = z2;
            p2->ae = z2;
            p2->c4 = z2;
            p2->b0 = 1;
            if (gUnk_030053F4 & 0x1000)
                p2->h356 = z2;
            else
                p2->h356 = 0x5a;
        }
    } else {
    inc:
        p->c4++;
    }
}

struct S_0802461C {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 ab;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_0802461C(struct S_0802461C *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void SceneActorState_PlayVariantAnimation13ThenRecover(struct S_0802461C *p)
{
    struct S_0802461C *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk_0802461C(p, z))
        SceneActorState_SetValueClampedWhenActive(p2, 0xD);
    if (p->af) {
        if (p2->ab == 3) {
            u32 k = 0xD;
            p2->aa = k;
            goto inc;
        } else {
            u32 z2 = 0;
            p2->ac = z2;
            p2->ad = z2;
            p2->ae = z2;
            p2->c4 = z2;
            p2->b0 = 1;
            if (gUnk_030053F4 & 0x1000)
                p2->h356 = z2;
            else
                p2->h356 = 0x96;
        }
    } else {
    inc:
        p->c4++;
    }
}

typedef struct State_080246C8 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter_080246C8, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_080246C8;

typedef struct Actor_080246C8 {
    State_080246C8 base;
    u8 pad104[0x2A0];
    s32 f3a4;
    u8 pad3a8[0x630];
    void *pool;
} Actor_080246C8;

void ContactActorPool_Start();

static inline u8 enter_080246C8(State_080246C8 *p)
{
    if (p->enter_080246C8) {
        p->enter_080246C8 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_StartContactPool(Actor_080246C8 *a)
{
    State_080246C8 *p = &a->base;
    struct ContactPosition pos;
    struct ContactPosition pos2;
    u8 *src = (u8 *)p + 0x48;

    if (enter_080246C8(p)) {
        u32 k = 0xE;
        p->animation = k;
    }
    if (p->finished) {
        struct ContactPosition *pp = &pos;
        struct ContactPosition *pp2;
        pos = *(struct ContactPosition *)(src + 0xC);
        pp->y += 200;
        pp2 = &pos2;
        pos2 = *pp;
        {
            s32 v = p->hp;
            a->f3a4 = v;
        }
        if (a->pool)
            ContactActorPool_Start(a->pool, pp, pp2, 10, 10);
        {
            u32 next = 0xC;
            p->phase = next;
        }
        p->elapsed = 0;
        p->enter_080246C8 = 1;
    } else
        p->elapsed++;
}

void SceneActorState_HoldAnimation15(struct StateActor *p)
{
    if (TakeFlag(p)) {
        u32 v = 0xF;
        p->actionAnimation = v;
    }
    p->actionTimer++;
}

struct S_080247AC {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_080247AC(struct S_080247AC *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_UpdateLongRecovery(struct S_080247AC *p)
{
    struct S_080247AC *p2 = p;
    if (chk_080247AC(p)) {
        u32 k = 0x10;
        p->aa = k;
    }
    if (p->af) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000)
            p2->h356 = z2;
        else
            p2->h356 = 0x5a;
    } else
        p->c4++;
}

// confidence=low: variant-dependent recovery animations and timed return to the default action.
#define SceneActorState_UpdateVariantRecovery sub_0802483C
// MATCHING_CONCESSION: register-pinning
struct VariantRecoveryActor {
    u8 pad0[0xaa];
    u8 animation, state, mode, index, phase, finished, enter;
    u8 padb1[0x13];
    u32 timer;
    u8 padc8[0x28e];
    u16 delay;
    u32 kind;
    u8 pad35c[0x50];
    u32 flags;
    u8 pad3b0[9];
    u8 alternate;
};
extern u32 gUnk_030053F4;
static inline u8 enterVariant(struct VariantRecoveryActor *p)
{
    if (p->enter) {
        p->enter = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}
static inline u8 variantAlternate(struct VariantRecoveryActor *q, u32 offset)
{
    u32 adjusted = offset + 0x61;
    {
        u8 *address = (u8 *)q + adjusted;
        return *address;
    }
}
void SceneActorState_UpdateVariantRecovery(struct VariantRecoveryActor *p)
{
    struct VariantRecoveryActor *q = p;
    if (enterVariant(p)) {
        u32 offset = 0x358;
        if (*(u32 *)((u8 *)q + offset) == 3 && variantAlternate(q, offset))
            SceneActorState_SetValueClampedWhenActive(q, 38);
        else
            SceneActorState_SetValueClampedWhenActive(q, 17);
    }
    if (p->finished) {
        if (q->state == 3) {
            u32 animation;
            u32 offset = 0x358;
            if (*(u32 *)((u8 *)q + offset) == 3 && variantAlternate(q, offset)) {
                animation = 38;
                asm("" : "+r"(animation));
            } else {
                animation = 17;
            }
            q->animation = animation;
        } else if (p->state == 17) {
            register u32 animation asm("r0");
            if (q->flags & 4) {
                animation = 19;
            } else {
                animation = 18;
            }
            p->animation = animation;
        } else {
            u32 zero = 0;
            q->mode = zero;
            q->index = zero;
            q->phase = zero;
            q->timer = zero;
            q->enter = 1;
            if (gUnk_030053F4 & 0x1000)
                q->delay = zero;
            else
                q->delay = 90;
            return;
        }
    }
    p->timer++;
}

struct S_08024944 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 ab;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024944(struct S_08024944 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void SceneActorState_UpdateAction20Recovery(struct S_08024944 *p)
{
    struct S_08024944 *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk_08024944(p, z))
        SceneActorState_SetValueClampedWhenActive(p2, 0x14);
    if (p->af) {
        if (p2->ab == 3) {
            u32 k = 0x14;
            p2->aa = k;
            goto inc;
        } else {
            u32 z2 = 0;
            p2->ac = z2;
            p2->ad = z2;
            p2->ae = z2;
            p2->c4 = z2;
            p2->b0 = 1;
            if (gUnk_030053F4 & 0x1000)
                p2->h356 = z2;
            else
                p2->h356 = 0x5a;
        }
    } else {
    inc:
        p->c4++;
    }
}

struct S_080249F0 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 ab;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_080249F0(struct S_080249F0 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

void SceneActorState_UpdateAction21Recovery(struct S_080249F0 *p)
{
    struct S_080249F0 *p2 = p;
    u32 z;
    u8 *a = &p->a9a;
    z = 0;
    *a = 1;
    if (chk_080249F0(p, z))
        SceneActorState_SetValueClampedWhenActive(p2, 0x15);
    if (p->af) {
        if (p2->ab == 3) {
            u32 k = 0x15;
            p2->aa = k;
            goto inc;
        } else {
            u32 z2 = 0;
            p2->ac = z2;
            p2->ad = z2;
            p2->ae = z2;
            p2->c4 = z2;
            p2->b0 = 1;
            if (gUnk_030053F4 & 0x1000)
                p2->h356 = z2;
            else
                p2->h356 = 0x5a;
        }
    } else {
    inc:
        p->c4++;
    }
}

void SceneActorState_UpdateAction22(struct StateActor *p)
{
    if (TakeFlag(p)) {
        u32 v = 0x16;
        p->actionAnimation = v;
    }
    p->actionTimer++;
}

struct S_08024AD4 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024AD4(struct S_08024AD4 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_UpdateShortRecovery(struct S_08024AD4 *p)
{
    struct S_08024AD4 *p2 = p;
    if (chk_08024AD4(p)) {
        u32 k = 0x17;
        p->aa = k;
    }
    if (p->af) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000)
            p2->h356 = z2;
        else
            p2->h356 = 0x1e;
    }
    p->c4++;
}

struct S_08024B60 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024B60(struct S_08024B60 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_WaitForFlagRelease(struct S_08024B60 *p)
{
    struct S_08024B60 *p2 = p;
    int one;
    if (chk_08024B60(p)) {
        u32 k = 0x18;
        p->aa = k;
    }
    one = 1;
    {
        u32 mask = one;
    }
    if (!((*((u32 *)(((u8 *)p) + 0x9c))) & 1)) {
        u32 z2 = 0;
        p2->ac = z2;
        p2->ad = z2;
        p2->ae = z2;
        p2->c4 = z2;
        p2->b0 = 1;
        if (gUnk_030053F4 & 0x1000) {
            p2->h356 = z2;
        } else {
            p2->h356 = 0x1e;
        }
    } else {
        p->c4++;
    }
}

struct S_08024BEC {
    u8 f0[0xb0];
    u8 b0;
    u8 f1[0x13];
    u32 c4;
};

void SceneActorState_UpdatePassiveAction(struct S_08024BEC *p)
{
    u8 *q = &p->b0;
    if (*q) {
        *q = 0;
        q--;
        *q = 0;
    }
    p->c4++;
}

extern const u32 gUnk_08604E9C[];

void SceneActorState_DispatchPrimaryAction(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604E9C[p->actionPhase]);
}

extern const u32 gUnk_08604EE0[];

void SceneActorState_DispatchSecondaryAction(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604EE0[p->actionPhase]);
}

extern u32 gUnk_02000560;

extern const u32 gUnk_08604EF0[];

void SceneActorState_UpdateActiveActions(struct StateActor *p)
{
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (SceneActorState_TryStartDeathOrFlaggedAction(p) == 0) {
            Callback_InvokeActor(p, gUnk_08604EF0[p->actionIndex]);
        }
    }
}

extern const u32 gUnk_08604EF4[];

void SceneActorState_UpdateSecondaryActions(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604EF4[p->actionIndex]);
}

extern const u32 gUnk_08604EF8[];

void SceneActorState_UpdatePassiveActions(struct StateActor *p)
{
    Callback_InvokeActor(p, gUnk_08604EF8[p->actionIndex]);
}

extern u32 gUnk_08604EFC[];

void SceneActorState_UpdateActionController(struct StateActor *p)
{
    /* Raw offset: 0xA1 is still unknown in state_actor.h (within unkA0). */
    ((u8 *)p)[0xa1] = 0;
    Callback_InvokeActor(p, gUnk_08604EFC[p->actionMode]);
}

typedef struct ActorAnimation_08024CE0 {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[7];
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    void (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    u16 scale, minScale, maxScale;
    u8 pad34A[0x62];
    u32 mode;
} ActorAnimation_08024CE0;

static inline void setFacing_08024CE0(ActorAnimation_08024CE0 *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}

// MATCHING_CONCESSION: register-pinning
void SceneActorAnimation_UpdateDirectionalCallback(ActorAnimation_08024CE0 *p)
{
    ActorAnimation_08024CE0 *p2 = p;
    if (!p->locked)
        setFacing_08024CE0(p, p->requestedAngle);
    if (p->changed) {
        int sequence;
        p->changed = 0;
        if (p->locked)
            setFacing_08024CE0(p, p->savedAngle);
        {
            int test = p->mode & 1;
            sequence = 43;
            if (test)
                sequence = 5;
        }
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1, 1,
                                              4);
    } else {
        int sequence;
        {
            int test = p->mode & 1;
            sequence = 43;
            if (test)
                sequence = 5;
        }
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1, 1,
                                              1);
    }
    p2->minScale = 0;
    p2->maxScale = 0;
    p2->variant = 0;
    if (!p->tryUpdate(p)) {
        u32 mask = 0x40;
        p->flags |= mask;
        p->reset(p);
    }
}

typedef struct ActorAnimation_08024E18 {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[7];
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    void (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    u16 scale, minScale, maxScale;
    u8 pad34A[0x62];
    u32 mode;
} ActorAnimation_08024E18;

static inline void setFacing_08024E18(ActorAnimation_08024E18 *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}

void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);

// MATCHING_CONCESSION: register-pinning
void SceneActorAnimation_UpdateEightDirectionCallback(ActorAnimation_08024E18 *p)
{
    if (!p->locked)
        setFacing_08024E18(p, p->requestedAngle);
    if (p->changed) {
        int sequence;
        p->changed = 0;
        if (p->locked)
            setFacing_08024E18(p, p->savedAngle);
        {
            int test = p->mode & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        SceneActors_StartEightDirectionAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1,
                                                 1, 4);
    } else {
        int sequence;
        {
            int test = p->mode & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        SceneActors_StartEightDirectionAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1,
                                                 1, 1);
    }
    if (!p->tryUpdate(p)) {
        u32 mask = 0x40;
        p->flags |= mask;
        p->reset(p);
    }
}
