#include "global.h"
#include "bosses/count/count_graphics.h"
#include "scene/collision/contact_actor.h"
#include "engine/video/render.h"
#include "engine/engine_globals.h"
#include "player/player.h"
#include "player/player_slot_status.h"
#include "engine/ram.h"
#include "debug.h"
#include "engine/math/hit_spark_random.h"
#include "bosses/state_actor_action_entry.h"
#include "scene/collision/contact_position.h"
#include "scene/collision/contact_reaction_vector_layout.h"
#include "scene/collision/packed_contact_vector.h"

int SceneActorGraphics_GetResourceId(struct StateActor *p)
{
    // concession-ok(duplicate-branches): every lock state names resource 0x335; the ROM still loads it
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
void CpuSet(const void *, void *, u32);
static inline void DispatchSelectPalette(struct T *t, s32 r)
{
    if (r == 0)
        t->palette = t->pal5C4;
    else if (r == 1)
        t->palette = t->pal7C4;
    else {
        CpuSet(gPaletteBankData + r * 32, t->pal3C4, 0x04000008);
        t->palette = t->pal3C4;
    }
}

static inline void SceneActorGraphics_SelectPalette(struct SceneGraphicsActor *actor, s32 bank)
{
    if (bank == 0)
        actor->animation.palette = (u16 *)actor->graphics.paletteBank1;
    else if (bank == 1)
        actor->animation.palette = (u16 *)actor->graphics.paletteBank2;
    else {
        CpuSet(gPaletteBankData + bank * 32, actor->graphics.paletteBank0, 0x04000008);
        actor->animation.palette = (u16 *)actor->graphics.paletteBank0;
    }
}

void SceneActorGraphics_SelectResource(struct SceneGraphicsActor *actor)
{
    if (actor->graphics.effectMode) {
        actor->graphics.effectScale = 0;
        actor->graphics.effectMode = 0;
        actor->graphics.resourceId =
            SceneActorGraphics_GetResourceId((struct StateActor *)actor);
        SceneActorGraphics_SelectPalette(actor,
                                         SceneActorGraphics_GetResourceId((struct StateActor *)actor));
    }
}
void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void Sound_PlaySongChecked(u32);
void DamageNumberDisplay_Show(void *, u32, u32, void *, void *, u32);
s32 Math_Atan2(s32, s32);
s32 WirelessGame_TryAddCyclingItem(void *);
void WirelessGame_AddItem(void *, s32);

void SceneActorGraphics_OnDamageContact(struct ContactCollider *source,
                                        struct ContactCollider *contact,
                                        void *owner)
{
    struct SceneGraphicsActor *actor = owner;
    struct SceneGraphicsActor *victim = owner;
    struct SceneActorBody *body = &actor->body;
    u32 mode;
    u32 flags;
    struct ContactReactionVector size;
    if (actor->graphics.hitImmunityFrames)
        return;
    mode = gPlayers[0]->actionMode;
    Collision_CalculateDamageResponse(source, contact);
    if (actor->graphics.stepTableKind == 1 && mode != 2)
        contact->cost = 0;
    if (!(gEngineFlags & 0x1000))
        PlayerStats_AddContactHundredths(source, contact, actor->graphics.contactStatHundredths);
    actor->pendingDamage = contact->cost;
    if (contact->cost == 0) {
        Sound_PlaySongChecked(0x211);
        actor->graphics.paletteFlashFrames = 10;
        actor->animation.palette = (u16 *)actor->graphics.paletteBank1;
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(contact, contact->cost, 0, &body->position, &size, 1);
    }
    if (ContactCollider_TestFlags(source, 0x100))
        actor->hitAngle = source->direction;
    else
        actor->hitAngle = Math_Atan2(contact->x - source->x, contact->z - source->z);
    flags = source->statRewardFlags;
    victim->hitStatRewardFlags = flags;
    flags = source->reactionFlags;
    victim->hitReactionFlags = flags;
    actor->graphics.hitImmunityFrames = source->responseValue;
    PlayerStats_AddContactHundredths(source, contact, victim->contactHundredths);
    if (actor->pendingDamage > 0 && ContactCollider_TestReactionFlags(source, 1))
        actor->damagePending = 1;
    if (!(gEngineFlags & 0x1000))
        return;
    if (mode == 1 || mode == 2) {
        if (WirelessGame_TryAddCyclingItem(&contact->position) && (contact->costFlags & 8))
            WirelessGame_AddItem(&contact->position, 0);
        return;
    }
    if (contact->costFlags & 8)
        WirelessGame_AddItem(&contact->position, 0);
    if (!ContactCollider_TestReactionFlags(source, 0x800000))
        return;
    switch ((s16)(*(gGameState->slots88 + gGameState->selected) - 6)) {
        case 0:
            WirelessGame_AddItem(&contact->position, 2);
            break;
        case 1:
            WirelessGame_AddItem(&contact->position, 1);
            break;
        case 2:
            WirelessGame_AddItem(&contact->position, 3);
            break;
        case 3:
            WirelessGame_AddItem(&contact->position, 2);
            WirelessGame_AddItem(&contact->position, 1);
            break;
        case 4:
            WirelessGame_AddItem(&contact->position, 2);
            WirelessGame_AddItem(&contact->position, 3);
            break;
        case 5:
            WirelessGame_AddItem(&contact->position, 2);
            WirelessGame_AddItem(&contact->position, 1);
            WirelessGame_AddItem(&contact->position, 3);
            break;
    }
}
void SceneActorGraphics_InitBodyNoOp(void)
{
}

typedef struct ContactPosition Vec;

void SceneActorGraphics_InitBodyNoOp(void);

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
        int test = gEngineFlags & 0x1000;
        int flags;
        flags = 0;
        if (test)
            flags = 0x40800;
        Collider_SetAttackProperties(
            drawable, ((struct SceneGraphicsActor *)p)->graphics.attackWidth0,
            ((struct SceneGraphicsActor *)p)->graphics.attackHeight0, 0, flags,
            ((struct SceneGraphicsActor *)p)->graphics.attackDepth0);
    }
}

void SceneActorGraphics_OnDamageContact(struct ContactCollider *, struct ContactCollider *,
                                        void *);

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
    Collider_SetCallback(drawable, SceneActorGraphics_OnDamageContact, p);
    Collider_SetBodyProperties(drawable, resource, 0x802, 1);
    Collider_Refresh(drawable);
}

void SceneActorGraphics_UpdateVariant(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct ContactCollider *drawable = (struct ContactCollider *)p->bodyDrawable;
    {
        if (SceneActorState_TestFlags(p, 0x40000))
            actor->graphics.variantToggle = 1;
        else
            actor->graphics.variantToggle = 0;
    }
    if (actor->graphics.variantToggle != actor->graphics.variantTogglePrev) {
        switch (actor->graphics.variantToggle) {
            case 0:
                Collider_SetAttackProperties(drawable, actor->graphics.attackWidth0,
                                             actor->graphics.attackHeight0, 0, 0,
                                             actor->graphics.attackDepth0);
                break;
            case 1:
                Collider_SetAttackProperties(drawable, actor->graphics.attackWidth1,
                                             actor->graphics.attackHeight1, 0, 0x60000,
                                             actor->graphics.attackDepth1);
                break;
        }
        {
            int value = actor->graphics.variantToggle;
            actor->graphics.variantTogglePrev = value;
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

s32 SceneActorGraphics_GetEffectState(struct StateActor *p)
{
    s32 r;
    if (gPlayerEffectPendingCount != 0) {
        r = 4;
    } else if (AreaEffect_IsPositionCovered(&p->target) == 0) {
        r = 0;
    } else {
        r = gGameState->unk876;
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

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);

void SceneActors_StartDisappearEffect(u32 *position)
{
    struct StateActor *p = SceneActors_GetBySlot(0);
    if (p) {
        int sequence;
        SceneActorState_StartAction(p, 0, 0, 13);
        p->target = *(struct StateActorPos *)position;
        SceneActors_SetFacing(p, p->requestedAngle);
        sequence = (((struct SceneGraphicsActor *)p)->graphics.animationFlags & 1) ? 24 : 62;
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1, 1,
                                              6);
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
            ((struct SceneGraphicsActor *)p)->graphics.healthCostAccumulator += amount;
            remaining =
                p->health - ((struct SceneGraphicsActor *)p)->graphics.healthCostAccumulator;
            if (remaining <= 1)
                goto stop;
            {
                diff = remaining - ((struct SceneGraphicsActor *)p)->graphics.healthCostBaseline;
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
            ((struct SceneGraphicsActor *)p)->graphics.resourceCostAccumulator += amount;
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

// Finds the first tracked position inside the inclusive x/z bounds.
s32 sub_08022624(void *actor, s32 count, struct BoxSearchPosition *min,
                 struct BoxSearchPosition *max)
{
    u8 *bytes = actor;
    s32 i;
    for (i = 0; i < count; i++) {
        s32 offset = i * 4;
        struct BoxSearchPosition *pos;
        u32 address = (u32)(bytes + 0x110);

        address += offset;
        ReadPositionReference((u32 *)address, &pos);
        if (pos) {
            s32 x = pos->x;
            s32 minX = min->x;
            s32 inside = 0;
            if (x >= minX && x <= max->x && pos->z >= min->z && pos->z <= max->z)
                inside = 1;
            if (inside)
                return i;
        }
    }
    return -1;
}

typedef struct ParticleOffset {
    s16 x, y, z, pad;
} ParticleOffset;
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
        const u16 *table = gRandomTable;
        u16 index = (gRandomIndex + 1) & 0x3FF;
        {
            u16 random = table[index];
            offset.x = ((random & 0x7F) + center.x) - 0x40;
        }
        offset.y = center.y;
        index = (index + 1) & 0x3FF;
        gRandomIndex = index;
        {
            u16 random = table[index];
            offset.z = ((random & 0x7F) + center.z) - 0x40;
        }
        table = 0;
        SceneParticle_CreateParticles(base + 0xC, &offset, table, table, 1, 1);
    }
}

// confidence=low: five sparks 300 units above the actor; the large burst for big, otherwise a
// smaller one shifted by the facing kept at 0xA8.
void sub_08022790(u8 *p, s32 big)
{
    struct ContactPosition pos;
    struct PackedContactVector size;
    struct ContactPosition *at = &pos;
    s32 angle, count, speed;
    u16 index;
    u32 *indexPtr;
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
    table = gRandomTable;
    indexPtr = &gRandomIndex;
    index = *indexPtr + 1;
    index &= 1023;
    angle = (u8)table[index];
    index = (index + 1) & 1023;
    count = table[index];
    smallMask = 3;
    count = (count & smallMask) + 2;
    index = (index + 1) & 1023;
    *indexPtr = index;
    speed = (table[index] & smallMask) + 20;
    SparkParticle_Spawn(&pos, 5, &size, angle, count, speed, 0, 2);
}

typedef struct PackedContactVector BurstVec;
extern const BurstVec gUnk_08604D84[];
u16 Sqrt(u32);
s32 Div(s32, s32);
void ImpactBurst_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
void TransientSpriteEffects_Spawn(int, int, void *, void *, int, int);

/* Emit particles away from a point above the center of the burst. */
static inline void BurstNormalize(struct ContactPosition *v, s32 speed)
{
    struct ContactPosition *out = v;
    u16 len = Sqrt(v->x * v->x + out->y * out->y + out->z * out->z);
    if (len) {
        v->x = Div(v->x * speed, len);
        out->y = Div(out->y * speed, len);
        out->z = Div(out->z * speed, len);
    }
}

static inline s32 BurstRandom(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

void sub_08022878(BurstVec *p)
{
    BurstVec center = *p;
    s32 i;
    BurstVec pos, vel;
    center.y += 0xc8;
    for (i = 0; i < 8; i++) {
        void *data = (void *)gRandomTable;
        pos.x = (s16)(p->x + gUnk_08604D84[i].x - 16) + (BurstRandom(data) & 0x1f);
        pos.y = (s16)(p->y + gUnk_08604D84[i].y - 16) + (BurstRandom(data) & 0x1f);
        pos.z = (s16)(p->z + gUnk_08604D84[i].z - 16) + (BurstRandom(data) & 0x1f);
        data = &pos;
        ImpactBurst_EmitBurst(data, 1, 0, 0x40, 0xc, 0xc, 4, 4);
        vel.x = pos.x - center.x;
        vel.y = pos.y - center.y;
        vel.z = pos.z - center.z;
        BurstNormalize((struct ContactPosition *)&vel, 5);
        TransientSpriteEffects_Spawn(2, 0, &pos, &vel, 0x20, 0x100);
    }
}

void SceneActors_SpawnAmbientParticlePair(struct StateActor *p)
{
    u8 *graphics = (u8 *)p + 0x240;
    struct PackedContactVector size;
    int i;
    for (i = 0; i < 2; i++) {
        u32 angle, count;
        u32 index;
        u32 *indexPtr;
        u32 value;
        u32 speed;
        size.x = 164;
        size.y = 16;
        size.z = 164;
        indexPtr = &gRandomIndex;
        value = (u32)gRandomTable;
        index = HitSpark_NextIndex(*indexPtr, 1023);
        angle = (u8)HitSpark_Sample(value, index);
        index = HitSpark_NextIndex(index, 1023);
        count = (HitSpark_Sample(value, index) & 3) + 2;
        index = HitSpark_NextIndex(index, 1023);
        *indexPtr = index;
        speed = (HitSpark_Sample(value, index) & 3) + 20;
        value = 3;
        SparkParticle_Spawn((struct ContactPosition *)(graphics + 0x20), value, &size, angle, count,
                            speed, 2, 2);
    }
}

void SceneActors_StopBackgroundFlash(struct StateActor *p)
{
    BackgroundFlash_BeginDisappearance(((struct SceneGraphicsActor *)p)->graphics.backgroundFlash);
    ((struct SceneGraphicsActor *)p)->graphics.backgroundFlashFlag = 0;
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
void SceneActors_UpdateShrinkingReaction(void *, int);
void SceneActors_UpdateKnockbackReaction(void *, int);
void sub_08027214(void *, int);
void WirelessGame_AddItem(void *, s32);

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
            if (((struct SceneGraphicsActor *)p)->graphics.stepTableKind == 1 &&
                (*(u8 **)gPlayers)[0x418] != 2) {
                shown = 0;
                result = 0;
            }
            if (damage > 0) {
                struct ContactReactionVector size;
                size.x = 0x20;
                size.y = 0;
                size.z = 0x20;
                DamageNumberDisplay_Show(actor + 0x1cc, shown, 0, sprite + 0xc, &size, 1);
            }
            {
                if (SceneActorState_TestFlags((struct StateActor *)p, 0x100000))
                    SceneActors_StopBackgroundFlash((struct StateActor *)actor);
            }
            SceneActorState_StartAction((struct StateActor *)p, 1, 0, 0);
            SceneActorState_EnterAction((struct StateActor *)p, SceneActors_UpdateShrinkingReaction,
                                        0x16);
            if (gEngineFlags & 0x1000) {
                u8 *position = sprite + 0xc;
                if (WirelessGame_TryAddCyclingItem(position)) {
                    s32 d =
                        actor[0x20e] - Math_Atan2(*(s16 *)(sprite + 0xc) - gGameState->positionX,
                                                  *(s16 *)(sprite + 0x10) - gGameState->positionZ);
                    if (d < 0)
                        d = -d;
                    if (d <= 0x20)
                        WirelessGame_AddItem(position, 0);
                }
            }
            break;
        }
        case 2:
            SceneActorState_StartAction((struct StateActor *)p, 1, 0, 1);
            SceneActorState_EnterAction((struct StateActor *)p, SceneActors_UpdateKnockbackReaction,
                                        0x17);
            {
                u8 v = sprite[9] + 0x80;
                ((struct SceneGraphicsActor *)p)->hitAngle = v;
            }
            break;
        case 1: {
            {
                u8 v = (*(u8 **)((u8 *)p + 0x10c))[0x2d];
                ((struct SceneGraphicsActor *)p)->hitAngle = v;
            }
            {
                SceneActorState_SetFlags((struct StateActor *)p, 2);
            }
            SceneActorState_StartAction((struct StateActor *)p, 1, 0, 3);
            SceneActorState_EnterAction((struct StateActor *)p, sub_08027214, 0x19);
            break;
        }
    }
    return result;
}

void SceneActorState_StepTableByte(struct StateActor *p, const u8 *tbl)
{
    u16 *cur = &((struct SceneGraphicsActor *)p)->graphics.stepTableValue;
    u16 old = *cur;
    u8 *col;
    u32 row;
    ((struct SceneGraphicsActor *)p)->graphics.stepTableValuePrev = old;
    row = ((struct SceneGraphicsActor *)p)->graphics.stepTableRow;
    col = &((struct SceneGraphicsActor *)p)->graphics.stepTableCol;
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
    switch (((struct SceneGraphicsActor *)p)->graphics.stepTableKind) {
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
static inline s32 SceneActorState_GetHealthRatio(const struct StateActor *actor)
{
    s32 ratio = actor->healthRatio;
    return ratio;
}

void SceneActorState_SelectReactionPhase(u8 *p)
{
    struct StateActor *state = (struct StateActor *)p;
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (actor->graphics.stepTableKind == 3) {
        if (actor->graphics.flashPhase) {
            SceneActorState_EnterPhase(state, 14);
            return;
        }
    } else {
        if (actor->graphics.flashPhase == 3)
            SceneActors_StopBackgroundFlash(p);
        else if (actor->graphics.flashPhase) {
            SceneActorState_EnterPhase(state, 14);
            return;
        }
    }
    switch (actor->graphics.stepTableValue) {
        case 0:
            SceneActorState_EnterPhase(state, 9);
            return;
        case 1:
            SceneActorState_EnterPhase(state, 10);
            return;
        case 2:
            if (actor->graphics.stepTableKind != 3 && SceneActorState_GetHealthRatio(state) <= 63) {
                actor->graphics.particleFlag = 1;
            } else {
                actor->graphics.particleFlag = 0;
            }
            SceneActorState_EnterPhase(state, 2);
            return;
        case 3:
            SceneActorState_EnterPhase(state, 11);
            return;
        case 4:
            if (SceneActorState_GetHealthRatio(state) <= 63) {
                SceneActorState_EnterPhase(state, 14);
                return;
            } else {
                SceneActorState_EnterPhase(state, 11);
                return;
            }
        case 5:
            SceneActorState_EnterPhase(state, 15);
            return;
        case 6:
            SceneActorState_EnterPhase(state, 16);
            return;
    }
}

// confidence=low: reaction-kind selector controls the far-point mode and motion timer.
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
            *(u16 *)(p + 0x354) = 64;
            return;
        case 1:
            Path_SnapToRandomFarPoint(position, sprite + 12, 2, 65536);
            *(u16 *)(p + 0x354) = 64;
            return;
        case 2:
            Path_SnapToRandomFarPoint(position, sprite + 12, 1, 65536);
            *(u16 *)(p + 0x354) = 32;
            return;
        case 3:
            Path_SnapToRandomFarPoint(position, sprite + 12, 3, 65536);
            *(u16 *)(p + 0x354) = 64;
            return;
        case 4:
            Path_SnapToRandomFarPoint(position, sprite + 12, 3, 65536);
            *(u16 *)(p + 0x354) = 64;
            return;
        case 5:
            Path_SnapToRandomFarPoint(position, sprite + 12, 3, 65536);
            *(u16 *)(p + 0x354) = 64;
            return;
        case 6:
            Path_SnapToRandomFarPoint(position, sprite + 12, 2, 65536);
            *(u16 *)(p + 0x354) = 64;
            return;
        default:
            *position = *(struct SnapVector *)(p + 0x54);
            break;
    }
    *(u16 *)(p + 0x354) = 64;
}

/* Scene commands set actor states, positions, graphics variants and background flashes. */
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

extern u8 *gSceneEventDispatchState;

void SceneMessages_SetPendingTrigger(void *, void *);

static inline void setStateFull(struct S *s, u32 state, u32 entered, u32 sub)
{
    s->state = state;
    s->entered = entered;
    s->sub = sub;
    s->timer = entered;
}

#define setState(s, sub) setStateFull(s, 2, 0, sub)

static inline void effect2(struct T *p, int mode, int id, int duration)
{
    p->w390 = id;
    p->b38E = mode;
    p->b38F = duration;
    if (p->b38E == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p->b38C = value;
    } else if (p->b38E == 1)
        p->b38C = 0;
}

#define effect(p, id, mode, d) effect2(p, mode, id, d)

void SceneActors_DispatchCommands(struct S *s)
{
    struct T *t = (struct T *)s;
    Entry *entry = &s->entry;
    s32 i;

    i = 0;
    if (i < entry->count[gSceneEventDispatchState[0x24]])
        do {
            s16 *m;
            u8 *controller = gSceneEventDispatchState;
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
                        t->b38C = 0;
                        t->b38E = 0;
                        t->w390 = SceneActorGraphics_GetResourceId(t);
                        DispatchSelectPalette(t, SceneActorGraphics_GetResourceId(t));
                    } else if (m[5] == 0) {
                        effect(t, 0x338, 3, 5);
                    } else {
                        effect(t, 0x33A, 3, 5);
                    }
                    Trigger_ConsumePending(&s->entry, 1);
                    break;
                case 16:
                    if (m[4] == 0)
                        effect(t, 0x335, 1, 5);
                    else if (m[5] == 0)
                        effect(t, 0x338, 2, 5);
                    else
                        effect(t, 0x33A, 2, 5);
                    Trigger_ConsumePending(&s->entry, 1);
                    break;
                case 9:
                    SceneActors_ConfigureEffect(s, m[4], 0xB4, 0xB4);
                    Trigger_ConsumePending(&s->entry, 1);
                    break;
                case 17:
                    t->b3B8 = m[4];
                    Trigger_ConsumePending(&s->entry, 1);
                    break;
                case 2:
                    s->h54 = m[4];
                    s->h56 = m[5];
                    s->h58 = m[6];
                    t->h260 = s->h54;
                    t->h262 = s->h56 + t->h344;
                    t->h264 = s->h58;
                    Trigger_ConsumePending(&s->entry, 1);
                    break;
                case 66:
                    t->b3BA = m[4];
                    setState(s, 21);
                    s->changed = 1;
                    break;
            }
            i++;
        } while (i < entry->count[gSceneEventDispatchState[0x24]]);
}

void SceneActorState_AdvanceToState1A(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1a);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

void SceneActorState_AdvanceToState01(struct StateActor *p)
{
    u8 entered = StateActor_ConsumeActionEntry(p);
    if (entered == TRUE) {
        p->actionAnimation = entered;
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

void SceneActorState_AdvanceToState02(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 2);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

void SceneActorState_Acknowledge(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 3);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState1B(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1b);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState1C(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1C);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_Resume(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1d);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 1);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState1E(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1e);
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 0);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState23(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x23);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState24(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x24);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState06(struct StateActor *p)
{
    struct SceneGraphicsActor *graphicsActor = (struct SceneGraphicsActor *)p;
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 6);
        graphicsActor->graphics.particleFlag = 0;
    }
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState20(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x20);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_AdvanceToState21(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x21);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_Hold(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x22);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    if (!p->actionFinished)
        p->actionTimer++;
}

void SceneActorState_PlayAnimation17AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x11);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation19AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x13);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation14ThenAdvance(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0xe);
    if (p->actionFinished) {
        SceneActorState_EnterPhase(p, 0x11);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation15AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0xf);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}

void SceneActorState_ResumeAfterAction(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x10);
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 1);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation31AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x1F);
    if (p->actionFinished)
        Trigger_ConsumePending(p->receiver2, 1);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation37ThenResume(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x25);
    if (p->actionFinished) {
        SceneActorState_StartAction(p, 2, 0, 0);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation39AndConsumeTrigger(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x27);
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

int SceneActorState_TryStartDeathOrFlaggedAction(struct StateActor *p)
{
    if (p->health <= 0 && p->vulnerable) {
        SceneActorState_SetFlags(p, 2);
        SceneActorState_EnterAction(p, sub_08027214, 25);
    }
    p->vulnerable = 0;
    if (SceneActorState_TestFlags(p, 2)) {
        SceneActorState_StartAction(p, 1, 0, 3);
        return 1;
    } else if (p->actionFlags & 1) {
        SceneActorState_StartAction(p, 1, 0, 2);
        return 1;
    }
    return 0;
}

void SceneActorState_WaitForDelayedAction(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p) == TRUE) {
        if (actor->variant == 0)
            p->actionAnimation = 1;
        else
            SceneActorState_SetAnimation(p, 2);
    }
    if (p->actionTimer > actor->graphics.recoveryTimer && SceneActorState_StepTableByKind(actor)) {
        SceneActorState_SnapReactionPosition((u8 *)actor,
                                             (struct SnapVector *)actor->graphics.unk9C4);
        SceneActorState_EnterPhase(p, 1);
    } else
        p->actionTimer++;
}

void SceneActorState_UpdateVariantActionPlayback(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenInactive(actor, 4);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState != 3) {
            SceneActorState_SelectReactionPhase((u8 *)actor);
            actor->graphics.recoveryTimer = 60;
            return;
        } else
            SceneActorState_SetAnimation((struct StateActor *)actor, 4);
    }
    p->actionTimer++;
}

void SceneActorState_PlayAnimation5ThenAdvance(struct StateActor *p)
{
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 5);
    if (p->actionFinished)
        SceneActorState_EnterPhase(p, 3);
    else
        p->actionTimer++;
}

void SceneActorState_EnterPhase6AndResetAnimation(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    SceneActorState_EnterPhase(p, 6);
    SceneActorState_SetAnimation(p, 9);
    p->actionReset(p);
    p->actionAnimation = 0;
    SceneActorState_ClearFlags(p, 0x40);
    actor->graphics.trackedPlayerEscaped = 0;
}

struct ProbeBox {
    s32 x : 16, y : 16, z : 16, w : 16;
};

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
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u8 *ProbeCellProps(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}

static inline u32 SceneActorState_TestPlayerFlags(const struct Player *player, u32 flags)
{
    return player->statusFlags & flags;
}

static inline bool32 ProbeHeightOutOfRange(s32 height, s32 referenceHeight, s32 minDelta,
                                           s32 maxDelta)
{
    u32 delta = height - referenceHeight - minDelta;
    u32 span = maxDelta - minDelta;
    return delta > span;
}

static inline bool32
SceneActorGraphics_IsForwardGroundOutOfRange(const struct SceneGraphicsActor *actor, s32 distance)
{
    struct ProbePos pos;
    u8 *s = (u8 *)actor + 0x48;
    u8 *props;
    struct ProbePos *at;
    u32 type;
    s32 h;
    ProbeProject(&pos, s[9], distance);
    ProbeAdd(&pos, (struct ProbePos *)(s + 0xc));
    at = &pos;
    props = ProbeCellProps(ProbeCellAt(at));
    type = *props >> 4;
    h = (*props & 0xf) << 8;
    switch (type) {
        case 1:
            h -= (u8)at->z;
            break;
        case 2:
            h -= (u8)at->x;
            break;
    }
    return ProbeHeightOutOfRange(h, actor->graphics.recoveryGroundHeight, -0x1ff, 0xff);
}

// confidence=low: grab a player in reach, or advance when the animation ends or the
// forward terrain height is outside the permitted interval.
void sub_08023FE8(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct ProbePos *sprite = (struct ProbePos *)((u8 *)p + 0x48);
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 6);
    if (actor->graphics.particleFlag == 1) {
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
        r = sub_08022624(actor, ((u8 *)actor)[0x174], (struct BoxSearchPosition *)&min,
                         (struct BoxSearchPosition *)&max);
        if (r >= 0) {
            struct Player *player = gPlayers[r];
            if (!SceneActorState_TestPlayerFlags(player, 0x40000)) {
                actor->graphics.trackedPlayer = r;
                SceneActorState_EnterPhase6AndResetAnimation(p);
                return;
            }
        }
    }
    if (SceneActorGraphics_IsForwardGroundOutOfRange(actor, 0x190) || p->actionFinished)
        SceneActorState_EnterPhase(p, 5);
    else
        p->actionTimer++;
}

static inline void SceneActorState_StartRecovery(struct SceneGraphicsActor *actor, u32 duration)
{
    SceneActorState_StartAction((struct StateActor *)actor, 0, 0, 0);
    if (gEngineFlags & 0x1000)
        actor->graphics.recoveryTimer = 0;
    else
        actor->graphics.recoveryTimer = duration;
}

void SceneActorState_PlayAnimation7ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 7);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 90);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation8ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 8);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 30);
    else
        p->actionTimer++;
}
// confidence=low: the tracked player escaping selects the next recovery phase.
void sub_080243CC(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p)) {
        DEBUG_PRINT();
    }
    if (p->actionFinished) {
        if (actor->graphics.trackedPlayerEscaped)
            SceneActorState_EnterPhase(p, 8);
        else
            SceneActorState_EnterPhase(p, 7);
    } else {
        p->actionTimer++;
    }
}

void SceneActorState_PlayAnimation10ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 10);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 30);
    else
        p->actionTimer++;
}

void SceneActorState_PlayAnimation11ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 11);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 30);
    else
        p->actionTimer++;
}

void SceneActorState_PlayVariantAnimation12ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenActive(actor, 12);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            SceneActorState_SetAnimation((struct StateActor *)actor, 12);
            goto tick;
        } else {
            SceneActorState_StartRecovery(actor, 90);
        }
    } else {
    tick:
        p->actionTimer++;
    }
}

void SceneActorState_PlayVariantAnimation13ThenRecover(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenActive(actor, 13);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            SceneActorState_SetAnimation((struct StateActor *)actor, 13);
            goto tick;
        } else {
            SceneActorState_StartRecovery(actor, 150);
        }
    } else {
    tick:
        p->actionTimer++;
    }
}
void ContactActorPool_Start();

void SceneActorState_StartContactPool(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    struct ContactPosition pos;
    struct ContactPosition pos2;
    u8 *src = (u8 *)&p->bodyId;

    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 14);
    if (p->actionFinished) {
        struct ContactPosition *pp = &pos;
        struct ContactPosition *pp2;
        pos = *(struct ContactPosition *)(src + 0xC);
        pp->y += 200;
        pp2 = &pos2;
        pos2 = *pp;
        {
            s32 health = p->health;
            actor->graphics.healthCostBaseline = health;
        }
        if (actor->graphics.contactPool)
            ContactActorPool_Start(actor->graphics.contactPool, pp, pp2, 10, 10);
        SceneActorState_EnterPhase(p, 12);
    } else
        p->actionTimer++;
}

void SceneActorState_HoldAnimation15(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 15);
    p->actionTimer++;
}
void SceneActorState_UpdateLongRecovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 16);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 90);
    else
        p->actionTimer++;
}
// confidence=low: variant-dependent recovery animations and timed return to the default action.
void SceneActorState_UpdateVariantRecovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    if (StateActor_ConsumeActionEntry(p)) {
        if (actor->graphics.stepTableKind == 3 && actor->graphics.flashPhase)
            SceneActorState_SetValueClampedWhenActive(actor, 38);
        else
            SceneActorState_SetValueClampedWhenActive(actor, 17);
    }
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            if (actor->graphics.stepTableKind == 3 && actor->graphics.flashPhase)
                SceneActorState_SetAnimation((struct StateActor *)actor, 38);
            else
                SceneActorState_SetAnimation((struct StateActor *)actor, 17);
        } else if (p->actionState == 17) {
            if (actor->graphics.animationFlags & 4)
                SceneActorState_SetAnimation(p, 19);
            else
                SceneActorState_SetAnimation(p, 18);
        } else {
            SceneActorState_StartRecovery(actor, 90);
            return;
        }
    }
    p->actionTimer++;
}

void SceneActorState_UpdateAction20Recovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenActive(actor, 20);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            SceneActorState_SetAnimation((struct StateActor *)actor, 20);
            goto tick;
        } else {
            SceneActorState_StartRecovery(actor, 90);
        }
    } else {
    tick:
        p->actionTimer++;
    }
}

void SceneActorState_UpdateAction21Recovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    p->vulnerable = 1;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetValueClampedWhenActive(actor, 21);
    if (p->actionFinished) {
        if (((struct StateActor *)actor)->actionState == 3) {
            SceneActorState_SetAnimation((struct StateActor *)actor, 21);
            goto tick;
        } else {
            SceneActorState_StartRecovery(actor, 90);
        }
    } else {
    tick:
        p->actionTimer++;
    }
}

void SceneActorState_UpdateAction22(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 22);
    p->actionTimer++;
}
void SceneActorState_UpdateShortRecovery(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 23);
    if (p->actionFinished)
        SceneActorState_StartRecovery(actor, 30);
    p->actionTimer++;
}
void SceneActorState_WaitForFlagRelease(struct SceneGraphicsActor *actor)
{
    struct StateActor *p = (struct StateActor *)actor;
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 24);
    if (!SceneActorState_TestFlags(p, 1))
        SceneActorState_StartRecovery(actor, 30);
    else
        p->actionTimer++;
}

void SceneActorState_UpdatePassiveAction(struct StateActor *p)
{
    StateActor_ConsumeActionEntry(p);
    p->actionTimer++;
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
    if (!PlayerSlot_ReadStatus(0, 4)) {
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

void SceneActorAnimation_UpdateDirectionalCallback(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (!p->locked)
        SceneActors_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (p->locked)
            SceneActors_SetFacing(p, p->savedFacing);
        {
            int test = actor->graphics.animationFlags & 1;
            sequence = 43;
            if (test)
                sequence = 5;
        }
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1, 1,
                                              4);
    } else {
        int sequence;
        {
            int test = actor->graphics.animationFlags & 1;
            sequence = 43;
            if (test)
                sequence = 5;
        }
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1, 1,
                                              1);
    }
    actor->graphics.scaleMin = 0;
    actor->graphics.scaleMax = 0;
    actor->variant = 0;
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 0x40);
        p->actionReset(p);
    }
}

void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);

void SceneActorAnimation_UpdateEightDirectionCallback(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (!p->locked)
        SceneActors_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (p->locked)
            SceneActors_SetFacing(p, p->savedFacing);
        {
            int test = actor->graphics.animationFlags & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        SceneActors_StartEightDirectionAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1,
                                                 1, 4);
    } else {
        int sequence;
        {
            int test = actor->graphics.animationFlags & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        SceneActors_StartEightDirectionAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1,
                                                 1, 1);
    }
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 0x40);
        p->actionReset(p);
    }
}
