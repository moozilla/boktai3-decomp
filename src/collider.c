#include "global.h"
#include "ram.h"
#include "contact_collider.h"
#include "collision_response.h"
#include "game_state.h"
#include "render.h"
#include "contact_collider_manager.h"



void Collider_Register(struct ContactCollider *n)
{
    struct ContactCollider *p;
    if (((struct ContactColliderManager *)gColliderManager) == 0)
        return;
    p = ((struct ContactColliderManager *)gColliderManager)->source;
    while ((p = p->next) != 0)
        if (p == n)
            return;
    ((struct ContactColliderManager *)gColliderManager)->sourceTail->next = n;
    n->next = 0;
    ((struct ContactColliderManager *)gColliderManager)->sourceTail = n;
    ((struct ContactColliderManager *)gColliderManager)->sourceCount++;
}


void Collider_InsertPriorityList(struct ContactCollider *node)
{
    struct ContactCollider *entry;
    if (!((struct ContactColliderManager *)gColliderManager))
        return;
    entry = ((struct ContactColliderManager *)gColliderManager)->pending->next;
    if (entry) {
        do {
            if (entry == node)
                return;
            entry = entry->next;
        } while (entry);
    }
    entry = ((struct ContactColliderManager *)gColliderManager)->pending;
    node->next = entry->next;
    entry->next = node;
    ((struct ContactColliderManager *)gColliderManager)->pendingCount++;
}

struct S {
    u8 a[6];
    u16 h6;
};



void Collider_Refresh(struct S *p)
{
    if (p->h6 & 0x2000) {
        if ((p->h6 & 4) == 0)
            Collider_InsertPriorityList(p);
    } else
        Collider_Register(p);
}

void Collider_Remove(struct ContactCollider *n)
{
    struct ContactCollider *prev, *cur;
    if (((struct ContactColliderManager *)gColliderManager) == 0)
        return;
    prev = ((struct ContactColliderManager *)gColliderManager)->source;
    cur = prev->next;
    while (cur) {
        if (cur == n) {
            if (cur == ((struct ContactColliderManager *)gColliderManager)->sourceTail)
                ((struct ContactColliderManager *)gColliderManager)->sourceTail = prev;
            prev->next = cur->next;
            ((struct ContactColliderManager *)gColliderManager)->sourceCount--;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
}

struct V {
    s16 x, y, z, pad;
};

struct Collider {
    u8 pad0[4];
    u16 id;
    u16 f6;
    u16 f8;
    u16 fa;
    u8 padc[8];
    struct V offset;
    struct V size;
    u8 pad24[0x34 - 0x24];
    u32 f34;
    u32 f38;
    u8 pad3c[2];
    u16 f3e;
    u16 f40;
    u8 pad42;
    u8 f43;
    u8 pad44[3];
    u8 f47, f48;
    u8 pad49[3];
    u32 f4c;
};

void Collider_Init(struct Collider *c, u16 id, u32 flags, u32 a, u32 b, struct V *size,
                   struct V *offset)
{
    c->f6 = flags;
    c->id = id;
    c->f8 = a;
    c->fa = b;
    if (size != 0)
        *(u64 *)&c->size = *(u64 *)size;
    else {
        c->size.x = 0;
        c->size.y = 0;
        c->size.z = 0;
    }
    if (offset != 0)
        *(u64 *)&c->offset = *(u64 *)offset;
    else {
        c->offset.x = 0;
        c->offset.y = 0;
        c->offset.z = 0;
    }
    c->f43 = 0;
    c->f3e = 0;
    c->f34 = 0;
    c->f38 = 0;
    c->f40 = 0;
    c->f4c = 0;
    c->f47 = 1;
    c->f48 = 0x40;
}

struct PositionCollider {
    u8 p[8];
    u16 f8;
    u16 pa;
    u16 fc;
    u16 fe;
    u16 f10;
};

void Collider_SetPosition(struct PositionCollider *o, u16 *s, u16 v)
{
    o->fc = s[0];
    o->fe = s[1];
    o->f10 = s[2];
    o->f8 = v;
}

struct Pair {
    u32 a, b;
};

void Collider_SetSegment(u8 *p, struct Pair *first, struct Pair *second, u16 value)
{
    *(struct Pair *)(p + 0xc) = *first;
    *(struct Pair *)(p + 0x14) = *second;
    *(u16 *)(p + 8) = value;
}

struct AttackCollider {
    u8 p[0x3c];
    u16 f3c;
    u16 pa;
    u16 f40;
};

void Collider_SetAttackPowerAndForce(struct AttackCollider *o, u16 a, u16 b)
{
    o->f3c = a;
    o->f40 = b;
}

void Collider_SetAttackProperties(struct ContactCollider *collider, s32 power, s32 knockbackPower,
                                  s32 weakness, u32 attributes, s32 invincibilityFrames)
{
    collider->unknown3C = power;
    collider->responseParameter = knockbackPower;
    collider->statRewardFlags = weakness;
    collider->reactionFlags = attributes;
    collider->responseValue = invincibilityFrames;
}

struct ContactColliderManagerodeCollider {
    u8 p[0x48];
    u8 f48;
};

void Collider_SetContactMode(struct ContactColliderManagerodeCollider *o, u8 a)
{
    o->f48 = a;
}

void Collider_SetBodyProperties(void *raw, u16 power, u32 reactionFlags, u32 rewardFlags)
{
    struct ContactCollider *collider = raw;
    collider->unknown3C = power;
    collider->reactionFlags = reactionFlags;
    collider->statRewardFlags = rewardFlags;
}

struct CallbackCollider {
    u8 p[0x4c];
    u32 f4c;
    u32 f50;
};

void Collider_SetCallback(struct CallbackCollider *o, u32 a, u32 b)
{
    o->f4c = a;
    o->f50 = b;
}

s32 Div(s32, s32);

static inline s32 ScaleSigned(s32 value)
{
    if (value >= 0)
        value >>= 6;
    else
        value = -(-value >> 6);
    return value;
}

static inline void ApplyScales(struct ContactCollider *a, struct ContactCollider *b)
{
    if (a->damageScale != 64) {
        if (a->statRewardFlags == 16) {
            s32 factor = gGameState->unk876;
            b->cost += Div((b->cost << 1) * factor, 100);
        } else
            b->cost = ScaleSigned(b->cost * a->damageScale);
    }
    if (b->damageScale != 64)
        b->cost = ScaleSigned(b->cost * b->damageScale);
    if (!b->cost)
        b->cost = 1;
}

static inline void Nearby(struct ContactCollider *a, struct ContactCollider *b, u32 amount)
{
    if ((b->flags & 0x100) && (a->flags & 0x100)) {
        s32 delta = b->direction - a->direction;
        if (delta < 0)
            delta = -delta;
        if (delta <= 32) {
            b->cost += amount >> 2;
            b->costFlags |= 8;
        }
    }
}

static inline u32 AttackFlags(struct ContactCollider *a, u32 mask)
{
    return a->reactionFlags & mask;
}

void Collision_CalculateDamageResponse(struct ContactCollider *a, struct ContactCollider *b)
{
    u32 damage;
    u32 defense, kind, attack;
    if (b->cost)
        return;
    if (AttackFlags(a, 0x1000))
        damage = a->unknown3C;
    else
        damage = a->unknown3C;
    if ((s32)damage <= 0)
        damage = 1;
    attack = a->reactionFlags;
    if (attack & 0x2000)
        defense = 0;
    else
        defense = b->reactionFlags & 63;
    kind = b->statRewardFlags & 63;
    if (attack & 64) {
        b->cost = 0;
        b->costFlags = 1;
        Nearby(a, b, b->cost);
    } else if (attack & 63) {
        if (kind & attack) {
            u32 v = (damage * 5) >> 5;
            b->cost = damage + v * 5;
            b->costFlags = 4;
        } else if (defense & attack) {
            u32 v = (damage * 5) >> 5;
            b->cost = damage - v * 5;
            b->costFlags = 2;
        } else {
            b->cost = damage + ((damage * 5) >> 5);
            b->costFlags = 1;
        }
        Nearby(a, b, damage);
        ApplyScales(a, b);
    } else {
        b->cost = damage;
        b->costFlags = 1;
        Nearby(a, b, damage);
        ApplyScales(a, b);
    }
}

struct DamageCollider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x2C];
    u32 resist;
    u32 attr;
    u16 power;
    u16 damage;
    u8 pad40[2];
    u8 height;
    u8 hitKind;
};

// Computes the damage `def` takes from `atk` (attribute vs. resistance), once per hit.

void Collision_CalculateElementalResponse(struct DamageCollider *atk, struct DamageCollider *def)
{
    s32 dmg;
    u32 attr, defAttr, resist;

    if (def->damage != 0)
        return;
    {
        u32 m = 0x1000;
        if (atk->attr & m)
            dmg = atk->power;
        else
            dmg = atk->power - def->power;
    }
    if (dmg <= 0)
        dmg = 1;
    attr = atk->attr;
    if (attr & 0x2000)
        defAttr = 0;
    else
        defAttr = def->attr & 0x7F;
    resist = def->resist & 0x7F;
    attr &= 0x7F;
    if (attr == 0) {
        def->damage = dmg;
        def->hitKind = 1;
    } else if (defAttr == 0) {
        def->damage = (dmg * 5) >> 2;
        def->hitKind = 1;
    } else if (defAttr == 0x40) {
        def->damage = (dmg * 3) >> 2;
        def->hitKind = 2;
    } else if (attr == 0x40) {
        def->damage = (dmg * 5) >> 2;
        def->hitKind = 1;
    } else if (defAttr & attr) {
        def->damage = (dmg * 3) >> 2;
        def->hitKind = 2;
    } else if (resist & attr) {
        def->damage = (dmg * 3) >> 1;
        def->hitKind = 4;
    } else {
        def->damage = (dmg * 5) >> 2;
        def->hitKind = 1;
    }
    if ((def->flags & 0x100) && (atk->flags & 0x100)) {
        s32 d = def->height - atk->height;
        if (d < 0)
            d = -d;
        if (d <= 0x20) {
            def->damage = (def->damage * 3u) >> 1;
            def->hitKind |= 8;
        }
    }
    if (def->damage == 0)
        def->damage = 1;
}

struct ContactBox {
    u8 pad0[0x1C];
    s16 ex, ey, ez; // half extents
    u8 pad22[2];
    s16 cx, cy, cz; // center
    u8 pad2a[0x18];
    u8 dir; // facing angle
};

struct ContactVec {
    s16 x, y, z;
};

// Collider contact helper: the push-out offset along the side `a` faces (+x, +z, -x, -z)

// that separates box `a` from box `b`.

void Collider_CalculateFacingSeparation(struct ContactBox *a, struct ContactBox *b, struct ContactVec *out)
{
    s32 side = ((a->dir + 0x20) & 0xFF) >> 6;

    if (side == 0) {
        out->x = a->cx + a->ex + b->ex - b->cx;
        out->y = side;
        out->z = side;
    } else if (side == 1) {
        out->x = 0;
        out->y = 0;
        out->z = a->cz + a->ez + b->ez - b->cz;
    } else if (side == 2) {
        out->x = a->cx - a->ex - b->ex - b->cx;
        out->y = 0;
        out->z = 0;
    } else {
        out->x = 0;
        out->y = 0;
        out->z = a->cz - a->ez - b->ez - b->cz;
    }
}

// Builds a center/half-extent box from two corner points.

struct HalfBox {
    u16 x, y, z;
};

struct BoxOut {
    u8 pad0[0x14];
    s16 f14, f16, f18;
    u8 pad1a[2];
    s16 ex, ey, ez;
    u8 pad22[2];
    s16 cx, cy, cz;
};

static inline s32 Half(s16 n)
{
    if (n >= 0)
        return n >> 1;
    return (s16) - ((-n) >> 1);
}

static inline s16 Abs16(s16 v)
{
    return v < 0 ? -v : v;
}

void Collider_BuildSegmentBounds(struct HalfBox *a, struct HalfBox *b, struct BoxOut *o)
{
    u16 t;
    t = Half(a->x - b->x);
    o->cx = Half(a->x + b->x);
    o->ex = Abs16(t);
    t = Half(a->y - b->y);
    o->cy = Half(a->y + b->y);
    o->ey = Abs16(t);
    t = Half(a->z - b->z);
    o->cz = Half(a->z + b->z);
    o->ez = Abs16(t);
    o->f14 = 0;
    o->f16 = 0;
    o->f18 = 0;
}

// Clips segment a->b against the box (center c, half-extents d) on its near

// face along axis 0; writes the hit point to out and returns 1 when the

// interpolated axes 1 and 2 fall inside the box.

u32 Collider_TestSegmentXAxis(s16 *a, s16 *b, s16 *c, s16 *d, s16 *out)
{
    s16 start = a[0];
    s16 delta = b[0] - start;
    s16 p, q, r;
    s16 center;

    if (delta == 0)
        return 0;
    center = c[0];
    if (center < start) {
        p = center + d[0];
        if (p > start)
            p = start;
    } else {
        p = center - d[0];
        if (p < start)
            p = start;
    }
    q = a[1] + Div((b[1] - a[1]) * (p - start), delta);
    if (q < c[1] - d[1] || q > c[1] + d[1])
        return 0;
    r = a[2] + Div((b[2] - a[2]) * (p - start), delta);
    if (r < c[2] - d[2] || r > c[2] + d[2])
        return 0;
    out[0] = p;
    out[1] = q;
    out[2] = r;
    return 1;
}

// Clips segment a->b against the box (center c, half-extents d) on its near

// face along axis 1; writes the hit point to out and returns 1 when the

// interpolated axes 2 and 0 fall inside the box.

u32 Collider_TestSegmentYAxis(s16 *a, s16 *b, s16 *c, s16 *d, s16 *out)
{
    s16 start = a[1];
    s16 delta = b[1] - start;
    s16 p, q, r;
    s16 center;

    if (delta == 0)
        return 0;
    center = c[1];
    if (center < start) {
        p = center + d[1];
        if (p > start)
            p = start;
    } else {
        p = center - d[1];
        if (p < start)
            p = start;
    }
    q = a[2] + Div((b[2] - a[2]) * (p - start), delta);
    if (q < c[2] - d[2] || q > c[2] + d[2])
        return 0;
    r = a[0] + Div((b[0] - a[0]) * (p - start), delta);
    if (r < c[0] - d[0] || r > c[0] + d[0])
        return 0;
    out[1] = p;
    out[2] = q;
    out[0] = r;
    return 1;
}

// Clips segment a->b against the box (center c, half-extents d) on its near

// face along axis 2; writes the hit point to out and returns 1 when the

// interpolated axes 0 and 1 fall inside the box.

u32 Collider_TestSegmentZAxis(s16 *a, s16 *b, s16 *c, s16 *d, s16 *out)
{
    s16 start = a[2];
    s16 delta = b[2] - start;
    s16 p, q, r;
    s16 center;

    if (delta == 0)
        return 0;
    center = c[2];
    if (center < start) {
        p = center + d[2];
        if (p > start)
            p = start;
    } else {
        p = center - d[2];
        if (p < start)
            p = start;
    }
    q = a[0] + Div((b[0] - a[0]) * (p - start), delta);
    if (q < c[0] - d[0] || q > c[0] + d[0])
        return 0;
    r = a[1] + Div((b[1] - a[1]) * (p - start), delta);
    if (r < c[1] - d[1] || r > c[1] + d[1])
        return 0;
    out[2] = p;
    out[0] = q;
    out[1] = r;
    return 1;
}




u32 Collider_TestSegmentBounds(const struct ContactPosition *start,
                               const struct ContactPosition *end, struct ContactCollider *collider,
                               void *output)
{
    const s16 *center = &collider->x;
    const struct ContactPosition *bounds = &collider->bounds;
    if (Collider_TestSegmentXAxis(start, end, center, bounds, output))
        return 1;
    if (Collider_TestSegmentYAxis(start, end, center, bounds, output))
        return 1;
    if (Collider_TestSegmentZAxis(start, end, center, bounds, output))
        return 1;
    return 0;
}

struct HitPoint {
    s32 a, b;
};

struct BoundsCollider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0xC];
    s16 offset[4];
    s16 ext[3];
    s16 pad;
    s16 pos[3];
    u8 pad2A[2];
    struct HitPoint hit;
    u8 pad34[0x54 - 0x34];
};



static inline s32 Box_OverlapsAxis(struct BoundsCollider *a, struct BoundsCollider *b, s32 i)
{
    s32 x = a->ext[i];
    s32 y = b->ext[i];
    s16 e = x + y;
    s32 hit = 0;
    s32 pa = a->pos[i];
    if (pa - e <= b->pos[i] && b->pos[i] - e <= pa)
        hit = 1;
    return hit;
}

static inline s32 Collider_BoxesOverlap(struct BoundsCollider *a, struct BoundsCollider *b)
{
    s32 result = 0;
    if (Box_OverlapsAxis(a, b, 0) && Box_OverlapsAxis(a, b, 1) && Box_OverlapsAxis(a, b, 2))
        result = 1;
    return result;
}

// Replace the segment candidate with its enclosing box for the broad-phase test.

static inline void BuildSegmentBounds(struct BoundsCollider **candidate, struct BoundsCollider *storage)
{
    struct BoundsCollider *segment = *candidate;

    Collider_BuildSegmentBounds(segment->pos, segment->offset, storage);
    *candidate = storage;
}

u32 Collider_TestSegmentCollision(struct BoundsCollider *a, struct BoundsCollider *b)
{
    struct BoundsCollider box;
    struct HitPoint hit;
    struct BoundsCollider *bounds = a;

    BuildSegmentBounds(&bounds, &box);
    if (!Collider_BoxesOverlap(bounds, b) || !Collider_TestSegmentBounds(a->pos, a->offset, b, &hit))
        return 0;

    if (!(b->flags & 2)) {
        b->hit = hit;
        a->hit = hit;
    } else {
        b->hit = hit;
    }
    return 1;
}

struct PositionNode {
    struct PositionNode *next;
    u16 unk4, flags;
    u8 pad8[4];
    u16 offset[3];
    u16 unk12;
    u16 base[3];
    u8 pad1A[10];
    u16 translated[3];
};

void PositionList_UpdateTranslatedCoordinates(struct PositionNode **first,
                                              struct PositionNode **second)
{
    struct PositionNode *next;
    struct PositionNode **saved;
    struct PositionNode *node;
    saved = second;
    next = *first;
    while (next) {
        node = next;
        next = next->next;
        node->translated[0] = node->offset[0] + node->base[0];
        node->translated[1] = node->offset[1] + node->base[1];
        node->translated[2] = node->offset[2] + node->base[2];
    }
    {
        struct PositionNode *node;
        struct PositionNode *next = *saved;
        while (next) {
            node = next;
            next = next->next;
            if (!(node->flags & 4)) {
                node->translated[0] = node->offset[0] + node->base[0];
                node->translated[1] = node->offset[1] + node->base[1];
                node->translated[2] = node->offset[2] + node->base[2];
            }
        }
    }
}



s32 Box_Overlaps(struct ContactCollider *, struct ContactCollider *);
s32 sub_08249240(void *, void *, void *);

void Collision_ProcessPairs(struct ContactCollider **listA, struct ContactCollider **listB)
{
    struct ContactCollider *a;
    struct ContactCollider *b;
    struct ContactCollider *cur;
    struct ContactCollider *other;
    s32 (*overlaps)(struct ContactCollider *, struct ContactCollider *) = Box_Overlaps;
    u32 suppressCallback, priorityMask;
    PositionList_UpdateTranslatedCoordinates(listA, listB);
    cur = *listA;
    while (cur != 0) {
        a = cur;
        cur = a->next;
        b = *listB;
        if (b != 0) {
            suppressCallback = 2;
            do {
                other = b;
                b = other->next;
                if ((a->contactFlags & other->contactFlags) != 0)
                    continue;
                if ((other->flags & 4) != 0)
                    continue;
                if (other->collisionId != 0 && other->collisionId == a->collisionId)
                    continue;
                if ((a->flags & 1) == 0 && (other->flags & 1) != 0)
                    continue;
                if ((a->flags & 0x1000) != 0 && (other->flags & 0x1000) == 0)
                    continue;
                if (overlaps(a, other) == 0)
                    continue;
                other->cost = 0;
                priorityMask = 0x200000;
                if ((a->statRewardFlags & priorityMask) != 0) {
                    if ((a->flags & suppressCallback) == 0 && other->callback != 0)
                        other->callback(a, other, other->owner);
                    if ((other->flags & suppressCallback) == 0 && a->callback != 0)
                        a->callback(a, other, a->owner);
                } else {
                    if ((other->flags & suppressCallback) == 0 && a->callback != 0)
                        a->callback(a, other, a->owner);
                    if ((a->flags & suppressCallback) == 0 && other->callback != 0)
                        other->callback(a, other, other->owner);
                }
            } while (b != 0);
        }
    }
}



s32 PositionQueue_FlushPending(struct ContactColliderManager *q)
{
    if (q->pendingCount) {
        if (gEnginePauseFlags) {
            u32 *pending;
            q->pendingCount = 0;
            pending = q->pending;
            *pending = 0;
            q->pendingTail = pending;
        } else {
            u32 *pending;
            if (q->sourceCount)
                Collision_ProcessPairs(q->pending, q->source);
            q->pendingCount = 0;
            pending = q->pending;
            *pending = 0;
            q->pendingTail = pending;
        }
    }
    return 0;
}

void Collider_OnManagerDestroy(void)
{
    gColliderManager = 0;
}

void Collider_ResetManagerLists(void)
{
    struct ContactColliderManager *manager = (struct ContactColliderManager *)gColliderManager;
    if (manager != NULL) {
        manager->source = &manager->sourceHead;
        manager->pending = &manager->pendingHead;
        manager->sourceHead.next = NULL;
        manager->pendingHead.next = NULL;
        manager->sourceTail = &manager->sourceHead;
        manager->pendingTail = &manager->pendingHead;
        manager->sourceCount = 0;
        manager->pendingCount = 0;
    }
}

u8 *Task_Create(u32, u32);

void Task_SetCallbacks(u8 *, void *, void *);


void Collider_OnManagerDestroy(void);

void Collider_ResetManagerLists(void);

u8 *Collider_EnsureManager(void)
{
    u8 *p;
    if (gColliderManager != 0)
        return gColliderManager;
    p = Task_Create(0xb, sizeof(struct ContactColliderManager));
    if (p != 0) {
        Task_SetCallbacks(p, PositionQueue_FlushPending, Collider_OnManagerDestroy);
        gColliderManager = p;
        Collider_ResetManagerLists();
    }
    return p;
}

struct ContactColliderManager *Collider_GetManager(void)
{
    return (struct ContactColliderManager *)gColliderManager;
}
