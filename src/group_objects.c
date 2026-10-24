// MATCHING_CONCESSION: register-pinning
#include "group_objects.h"

// MATCHING_CONCESSION: register-pinning
int BoardPiece_Initialize(struct GroupObjects *owner, struct GroupPiece *p, int index)
{
    struct GroupResource *resource;
    struct GroupSprite *sprite;
    struct GroupVec *position;
    {
        int zero = 0;
        asm("" : "+r"(zero));
        p->state = zero;
        p->active = 1;
        p->timer = 0;
        p->direction = zero;
    }
    resource = &p->resource;
    AnimData_Load(resource, 0xDA6D);
    {
        void *r = resource;
        asm("" : "+r"(r));
        AnimData_SelectPaletteBank(r, 0x2D3);
    }
    resource->palette = owner->palette;
    sprite = &p->sprite;
    Sprite_Init(sprite, resource, 1);
    sprite->mode = 2;
    position = &p->sprite.position;
    ZeroFill8(position);
    Shadow_Initialize(p->transform, position, 0);
    ActorShadow_Disable(p->transform);
}

u32 BoardPiece_Destroy(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    Shadow_Destroy(p->transform);
    Sprite_Remove(&p->sprite);
}

static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}

void BoardPiece_SetFacingAnim(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    u8 left;
    u8 right;
    u32 dir = (((u16)p->direction + 0x60) >> 6) & 3;
    u8 *l = &left;
    u8 *r = &right;
    if ((u8)(dir - 1) <= 1)
        *l = 1;
    else
        *l = 0;
    if (dir > 1)
        *r = 1;
    else
        *r = 0;
    Anim_SetSequence(&p->anim, m->animation, (u16)index, left, right);
}

// MATCHING_CONCESSION: register-pinning
void BoardPiece_UpdateAnimation(struct GroupPiece *p)
{
    struct GroupSprite *o = &p->sprite;
    struct GroupAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    int done;
    p->sprite.position = p->position;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                } else {
                    a->frame = 0;
                }
                done = 1;
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (p->position.y > p->targetY)
        p->position.y--;
    else if (p->position.y < p->targetY)
        p->position.y++;
    {
        int value = gUnk_086149C4[p->angle] * 32;
        {
            int delta;
            if (value >= 0)
                delta = value >> 12;
            else {
                int magnitude = -value;
                delta = -(magnitude >> 12);
            }
            o->position.y = o->position.y + delta;
        }
    }
    p->angle += 4;
}

static inline void addPositionAt(struct GroupVec *d, struct GroupVec *dst, struct GroupVec *src)
{
    d->x += src->x;
    dst->y += src->y;
    dst->z += src->z;
}

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
int sub_0800FC28(struct GroupObjects *m, struct GroupQueue *q, struct GroupCommand *cmd)
{
    struct GroupVec origin;
    int i;
    int radius0;
    int radius1;
    int radius2;
    int direction;
    int spread;
    struct GroupPiece *p;
    struct GroupPiece *r;
    u32 k1 = 16;
    origin.x = cmd->originX;
    origin.y = cmd->originY;
    origin.z = cmd->originZ;
    radius0 = cmd->radius0;
    radius1 = cmd->radius1;
    radius2 = cmd->radius2;
    direction = cmd->direction;
    spread = cmd->spread;
    m->origin = origin;
    m->timer = 0;
    m->paletteActive = 1;
    m->paletteMask = 63;
    {
        void *first = gUnk_030042E4 + 0x5A60;
        void *second = gUnk_030042E4 + 0x5AA0;
        Palette_InterpolateBanks(m->palette, first, second, 63, 6);
    }
    p = m->pieces;
    r = p;
    for (i = 0; i < k1; i++, p++, r++) {
        p->state = 5;
        p->active = 1;
        p->timer = 0;
        p->direction = direction;
        if (spread != 0)
            p->direction += Mod((s16)randomValue(), spread);
        p->duration = radius0;
        polar(&r->position, p->direction, radius1);
        addPositionAt(&p->position, &r->position, &origin);
        polar(&r->target, p->direction, radius2);
        addPositionAt(&p->target, &r->target, &origin);
        p->angle = randomValue();
        p->targetY = (i * k1) + p->position.y;
        p->speed = (randomValue() & 7) + 7;
        p->sprite.position = p->position;
        p->sprite.flags &= ~1;
        BoardPiece_SetFacingAnim(m, r, 0);
        AnimData_SelectPaletteBank(&r->resource, 0x2D3);
        p->resource.palette = m->palette;
        ActorShadow_Enable(r->transform);
    }

    m->active = 1;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0800FC28);
#endif
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
int sub_0800FE98(struct GroupObjects *m, struct GroupQueue *q, struct GroupCommand *cmd)
{
    struct GroupVec origin;
    int i;
    int radius0;
    int radius1;
    int radius2;
    int direction;
    int spread;
    struct GroupPiece *p;
    struct GroupPiece *r;
    origin.x = cmd->originX;
    origin.y = cmd->originY;
    origin.z = cmd->originZ;
    radius0 = cmd->radius0;
    radius1 = cmd->radius1;
    radius2 = cmd->radius2;
    direction = cmd->direction;
    spread = cmd->spread;
    m->origin = origin;
    m->timer = 0;
    m->paletteActive = 1;
    m->paletteMask = 63;
    {
        void *first = gUnk_030042E4 + 0x5A60;
        void *second = gUnk_030042E4 + 0x5AA0;
        Palette_InterpolateBanks(m->palette, first, second, 63, 6);
    }
    p = m->pieces;
    r = p;
    for (i = 0; i < 16; i++, p++, r++) {
        do {
            p->state = 0;
        } while (0);
        p->active = 1;
        p->timer = 0;
        p->direction = direction;
        if (spread != 0)
            p->direction += Mod(randomValue(), spread);
        p->duration = radius0;
        polar(&r->position, p->direction, radius1);
        addPositionAt(&p->position, &r->position, &origin);
        polar(&r->target, p->direction, radius2);
        addPositionAt(&p->target, &r->target, &origin);
        p->angle = randomValue();
        p->targetY = (i * 16) + p->position.y;
        p->speed = (randomValue() & 7) + 7;
        p->sprite.position = p->position;
        p->sprite.flags &= ~1;
        BoardPiece_SetFacingAnim(m, r, 0);
        AnimData_SelectPaletteBank(&r->resource, 0x2D3);
        p->resource.palette = m->palette;
        ActorShadow_Enable(r->transform);
    }

    m->active = 1;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0800FE98);
#endif
s32 Div(s32, s32);
struct PackedVec {
    u32 x : 16, y : 16, z : 16;
};

// confidence=low: converges the board pieces on the command origin at a scaled speed.
#define BoardPieces_StartConvergence sub_0801010C
int BoardPieces_StartConvergence(struct GroupObjects *m, struct GroupQueue *q,
                                 struct GroupCommand *cmd)
{
    struct PackedVec origin;
    struct GroupVec d;
    int i;
    int radius;
    void *palette;
    struct GroupPiece *p;
    struct GroupPiece *r;
    origin.x = cmd->originX;
    origin.y = cmd->originY;
    origin.z = cmd->originZ;
    radius = cmd->radius0;
    if (radius <= 0)
        radius = 1;
    *(struct PackedVec *)&m->origin = origin;
    m->timer = 0;
    r = m->pieces;
    i = 0;
    palette = m->palette;
    for (; i < 16; i++, r++) {
        int angle;
        struct GroupVec *dp = &d;
        struct GroupVec *o = (struct GroupVec *)&origin;
        p = r;
        {
            u32 state = 7;
            u32 zero = 0;
            p->state = state;
            p->active = 1;
            p->timer = zero;
        }
        dp->x = o->x - p->position.x;
        dp->y = o->y - p->position.y;
        dp->z = o->z - p->position.z;
        angle = Math_Atan2(dp->x, dp->z);
        p->duration = Div(
            scale(dp->x * gUnk_086149C4[(angle + 64) & 255] + dp->z * gUnk_086149C4[angle & 255]),
            radius);
        *(struct PackedVec *)&p->target = origin;
        {
            register const u16 *table asm("r2") = gUnk_0203B400;
            p->angle = randomWith(table);
            p->targetY = i * 16 + p->position.y;
            p->speed = (randomWith(table) & 7) + 7;
        }
        p->sprite.position = p->position;
        p->sprite.flags &= ~1;
        BoardPiece_SetFacingAnim(m, r, 0);
        AnimData_SelectPaletteBank(&r->resource, 0x2D3);
        p->resource.palette = palette;
        ActorShadow_Enable(r->transform);
    }
    m->active = 1;
    return 0;
}

static inline int BoardPieces_NextRandomValue(const u16 *table, u32 *index, u32 mask)
{
    u32 offset;
    *index = (*index + 1) & mask;
    offset = *index * 2;
    return *(const u16 *)(offset + (u32)table);
}

// MATCHING_CONCESSION: register-pinning
int sub_08010298(struct GroupObjects *m, struct GroupQueue *q, struct GroupCommand *cmd)
{
    struct GroupActor *actor;
    s32 duration;
    struct GroupVec position;
    int remaining;
    struct GroupPiece *p;
    u32 *index;
    u32 mask;
    m->actorId = cmd->originX;
    actor = SceneBody_GetById(m->actorId);
    if (!actor)
        return -1;
    position = actor->position;
    duration = (s16)cmd->originY;
    {
        register int enabled asm("r8");
        m->timer = 0;
        index = &gUnk_03005308;
        mask = 1023;
        remaining = 15;
        p = m->pieces;
    loop: {
        int angle;
        int zero;
        const u16 *table;
        p->state = 3;
        do {
            enabled = 1;
            p->active = enabled;
        } while (0);
        zero = 0;
        p->timer = zero;
        p->target = position;
        asm("" : : "r"(zero), "r"(index), "r"(mask));
        angle = Math_Atan2(p->target.x - p->position.x, p->target.z - p->position.z);
        p->direction = angle;
        table = gUnk_0203B400;
        {
            register int random asm("r3") = BoardPieces_NextRandomValue(table, index, mask);
            angle -= 16;
            random &= 31;
            angle += random;
            p->direction = angle;
        }
        p->duration = duration;
        p->speed = (BoardPieces_NextRandomValue(table, index, mask) & 7) + 7;
        remaining--;
        p++;
        if (remaining >= 0)
            goto loop;
    }

        m->active = enabled;
        return 0;
    }
}

static inline int count(struct GroupQueue *q)
{
    u8 *bank = &gUnk_0200047C->bank;
    return q->count[*bank];
}

int BoardPieces_DispatchCommands(struct GroupObjects *manager)
{
    struct GroupQueue *queue = &manager->queue;
    int i;
    for (i = 0; i < count(queue); i++) {
        struct GroupGlobals *g = gUnk_0200047C;
        {
            struct GroupCommand *cmd = queue->commands[g->bank][i];
            SceneMessages_SetPendingTrigger(queue, cmd);
            switch (cmd->type) {
                case 0:
                    sub_0800FC28(manager, queue, cmd);
                    Trigger_ConsumePending(&manager->queue, 1);
                    break;
                case 1:
                    sub_0800FE98(manager, queue, cmd);
                    Trigger_ConsumePending(&manager->queue, 1);
                    break;
                case 2:
                    sub_08010298(manager, queue, cmd);
                    Trigger_ConsumePending(&manager->queue, 1);
                    break;
                case 11:
                    sub_0801010C(manager, queue, cmd);
                    Trigger_ConsumePending(&manager->queue, 1);
                    break;
            }
        }
    }
    return 0;
}

static inline void addPosition(struct GroupVec *d, struct GroupVec *s)
{
    struct GroupVec *dst = d;
    struct GroupVec *src = s;
    {
        u32 value = (u16)s->x;
        {
            register u32 other asm("r3") = (u16)d->x;
            value = value + other;
        }
        d->x = value;
    }
    dst->y += src->y;
    dst->z += src->z;
}

static inline struct GroupVec *addPositionN(struct GroupVec *d, struct GroupVec *s)
{
    struct GroupVec *src = s;
    struct GroupVec *dst = d;
    d->x += s->x;
    dst->y += src->y;
    dst->z += src->z;
    return dst;
}

// MATCHING_CONCESSION: register-pinning

void BoardPiece_Launch(struct GroupObjects *m, struct GroupPiece *p, int delay)
{
    struct GroupVec delta;
    if (p->active)
        p->active = 0;
    polar(&delta, p->direction, p->duration * 2);
    addPosition(&p->position, &delta);
    BoardPiece_SetFacingAnim(m, p, 0);
    if (p->timer >= delay + 2) {
        p->state = 1;
        p->active = 1;
        p->timer = 0;
    }
    p->timer++;
}

void BoardPiece_Wander(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct GroupVec delta;
    struct GroupVec *moved;
    struct GroupVec *origin;
    const u16 *table;
    if (p->active) {
        p->active = 0;
        p->heading = p->direction =
            Math_Atan2(p->target.x - p->position.x, p->target.z - p->position.z);
        // agbcc weights a register's uses by loop depth: four empty loops make
        // `table` outrank the random-index address and mask, so it gets r2
        // (found by tools/translate.py; see docs/PERMUTER.md).
        do {
            do {
                do {
                    do {
                        table = gUnk_0203B400;
                    } while (0);
                } while (0);
            } while (0);
        } while (0);
        if (randomByte(table) <= 0x7F)
            p->heading = p->heading + (0x40 - (randomWith(table) & 0x3F));
        else
            p->heading = p->heading + (0x40 + (randomWith(table) & 0x3F));
        p->speed = (randomValue() & 3) + 4;
    }
    if (p->speed != 0) {
        polar(&delta, p->heading, p->duration);
        moved = addPositionN(&p->position, &delta);
        origin = &m->origin;
        p->direction = Math_Atan2(origin->x - p->position.x, origin->z - moved->z);
        p->speed--;
    }
    BoardPiece_SetFacingAnim(m, p, 0);
    if (p->timer > 0x5F) {
        u32 t = 1;
        p->state = t;
        p->active = 1;
        p->timer = 0;
    }
    p->timer++;
}
void BoardPiece_ApproachTarget(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct GroupVec delta;
    struct GroupVec *position;
    struct GroupVec *moved;
    int dx, dz;
    if (p->active) {
        const u16 *table;
        struct GroupVec *current;
        struct GroupVec *origin;
        p->active = 0;
        current = &p->position;
        origin = &m->origin;
        p->direction = Math_Atan2(origin->x - p->position.x, origin->z - current->z);
        p->heading = Math_Atan2(p->target.x - p->position.x, p->target.z - current->z);
        table = gUnk_0203B400;
        p->heading += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    position = &p->position;
    dx = p->target.x - p->position.x;
    dz = p->target.z - position->z;
    if (dx * dx + dz * dz <= 0x9C3F) {
        u32 t = 2;
        p->state = t;
        p->active = 1;
        p->timer = 0;
    } else {
        if (p->speed == 0) {
            const u16 *table;
            p->heading = Math_Atan2(dx, dz);
            table = gUnk_0203B400;
            p->heading += (randomWith(table) & 31) - 16;
            p->speed = (randomWith(table) & 31) + 32;
        } else
            p->speed--;
        polar(&delta, p->heading, p->duration);
        moved = addPositionN(&p->position, &delta);
        {
            struct GroupVec *origin = &m->origin;
            p->direction = Math_Atan2(origin->x - p->position.x, origin->z - moved->z);
        }
        BoardPiece_SetFacingAnim(m, p, 0);
        p->timer++;
    }
}
void BoardPiece_ConvergeOnTarget(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct GroupVec delta;
    struct GroupVec *position;
    struct GroupVec *moved;
    int dx, dz;
    if (p->active) {
        const u16 *table;
        struct GroupVec *current;
        struct GroupVec *origin;
        p->active = 0;
        current = &p->position;
        origin = &m->origin;
        p->direction = Math_Atan2(origin->x - p->position.x, origin->z - current->z);
        p->heading = Math_Atan2(p->target.x - p->position.x, p->target.z - current->z);
        table = gUnk_0203B400;
        p->heading += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    position = &p->position;
    dx = p->target.x - p->position.x;
    dz = p->target.z - position->z;
    if (dx * dx + dz * dz <= 0x9C3F) {
        u32 t = 8;
        p->state = t;
        p->active = 1;
        p->timer = 0;
    } else {
        if (p->speed == 0) {
            const u16 *table;
            p->heading = Math_Atan2(dx, dz);
            table = gUnk_0203B400;
            p->heading += (randomWith(table) & 31) - 16;
            p->speed = (randomWith(table) & 31) + 32;
        } else
            p->speed--;
        polar(&delta, p->heading, p->duration);
        moved = addPositionN(&p->position, &delta);
        {
            struct GroupVec *origin = &m->origin;
            p->direction = Math_Atan2(origin->x - p->position.x, origin->z - moved->z);
        }
        BoardPiece_SetFacingAnim(m, p, 0);
        p->timer++;
    }
}

void BoardPiece_Deactivate(struct GroupObjects *m, struct GroupPiece *p)
{
    if (p->active != 0) {
        p->active = 0;
        p->sprite.flags |= 1;
        ActorShadow_Disable(p->transform);
    }
}

void BoardPiece_ChasePlayer(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct GroupVec delta;
    struct GroupVec *position;
    struct GroupVec *target;
    int dx, dz;
    int active;
    if (m->actor)
        p->target = m->actor->position;
    active = p->active;
    position = &p->position;
    target = &p->target;
    if (active) {
        const u16 *table;
        p->active = 0;
        p->direction = Math_Atan2(target->x - position->x, target->z - position->z);
        table = gUnk_0203B400;
        p->direction += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    if (p->speed == 0) {
        const u16 *table;
        p->direction = Math_Atan2(target->x - position->x, target->z - position->z);
        table = gUnk_0203B400;
        p->direction += (randomWith(table) & 15) - 8;
        p->speed = (randomWith(table) & 15) + 16;
    } else
        p->speed--;
    dx = p->target.x - p->position.x;
    dz = target->z - position->z;
    if (dx * dx + dz * dz <= 0xFFF) {
        u32 t = 4;
        p->state = t;
        p->active = 1;
        p->timer = 0;
    } else {
        polar(&delta, p->direction, p->duration);
        {
            struct GroupVec *src = &delta;
            p->position.x += delta.x;
            position->y += src->y;
            position->z += src->z;
        }
        BoardPiece_SetFacingAnim(m, p, 0);
        p->timer++;
    }
}

void sub_08010C10(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct GroupVec delta;
    struct GroupVec *offset;
    int active;
    if (m->actor)
        p->target = m->actor->position;
    active = p->active;
    offset = &p->offset;
    if (active) {
        p->active = 0;
        p->anchor = p->position;
        p->direction = randomValue();
        ZeroFill8(offset);
    }
    Vec16_Lerp(&p->anchor, &p->anchor, &p->target, index + 8, 5);
    if (p->speed == 0) {
        const u16 *table;
        {
            register u32 d asm("r1");
            s32 ax;
            asm("" : "=r"(d));
            ax = -p->offset.x;
            asm("" ::"r"(d));
            p->direction = Math_Atan2(ax, -p->offset.z);
        }
        table = gUnk_0203B400;
        p->direction += (randomWith(table) & 15) - 7;
        p->speed = (randomWith(table) & 7) + 8;
    } else
        p->speed--;
    polar(&delta, p->direction, p->duration);
    {
        struct GroupVec *src = &delta;
        p->offset.x += delta.x;
        offset->y += src->y;
        offset->z += src->z;
    }
    p->position.x = p->anchor.x + p->offset.x;
    p->position.z = p->anchor.z + p->offset.z;
    BoardPiece_SetFacingAnim(m, p, 0);
    p->timer++;
}
void BoardPiece_AdvanceStraight(struct GroupObjects *m, struct GroupPiece *p, int delay)
{
    struct GroupVec delta;
    if (p->active)
        p->active = 0;
    polar(&delta, p->direction, p->duration * 2);
    addPosition(&p->position, &delta);
    BoardPiece_SetFacingAnim(m, p, 0);
    if (p->timer >= delay + 2) {
        p->state = 6;
        p->active = 1;
        p->timer = 0;
    }
    p->timer++;
}
void BoardPiece_HomeToTarget(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct GroupVec delta;
    struct GroupVec *position;
    int dx, dz;
    int active = p->active;
    if (active) {
        const u16 *table;
        p->active = 0;
        p->direction = Math_Atan2(p->target.x - p->position.x, p->target.z - p->position.z);
        table = gUnk_0203B400;
        p->direction += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    position = &p->position;
    dx = p->target.x - p->position.x;
    dz = p->target.z - position->z;
    if (dx * dx + dz * dz <= 0x9C3F) {
        u32 t = 2;
        p->state = t;
        p->active = 1;
        p->timer = 0;
    } else {
        if (p->speed == 0) {
            const u16 *table;
            p->direction = Math_Atan2(dx, dz);
            table = gUnk_0203B400;
            p->direction += (randomWith(table) & 31) - 16;
            p->speed = (randomWith(table) & 31) + 32;
        } else
            p->speed--;
        polar(&delta, p->direction, p->duration);
        addPositionN(&p->position, &delta);
        BoardPiece_SetFacingAnim(m, p, 0);
        p->timer++;
    }
}

int GroupObjects_Update(struct GroupObjects *m)
{
    int i;
    struct GroupPiece *p;
    BoardPieces_DispatchCommands(m);
    if (m->active) {
        if (m->actorId)
            m->actor = SceneBody_GetById(m->actorId);
        if (m->paletteActive == 0) {
            if (++m->paletteMask >= 63) {
                m->paletteMask = 63;
                m->paletteActive = -1;
            }
        } else if (m->paletteActive == 1) {
            if (--m->paletteMask <= 0) {
                m->paletteMask = 0;
                m->paletteActive = -1;
            }
        } else if (m->paletteActive == 2) {
            if (++m->paletteMask > 31) {
                m->paletteMask = 32;
                m->paletteActive = -1;
            }
        }
        if (m->paletteActive >= 0) {
            void *first = gUnk_030042E4 + 0x5A60;
            void *second = gUnk_030042E4 + 0x5AA0;
            Palette_InterpolateBanks(m->palette, first, second, m->paletteMask, 6);
        }
        p = m->pieces;
        for (i = 0; i < 16; i++, p++) {
            gUnk_08604B48[p->state](m, p, i);
            BoardPiece_UpdateAnimation(p);
        }
        m->timer++;
    }
    return 0;
}

int GroupObjects_Destroy(struct GroupObjects *m)
{
    struct GroupPiece *p = m->pieces;
    int i;
    for (i = 0; i < 16; i++, p++)
        BoardPiece_Destroy(m, p, i);
    SceneMessages_UnregisterReceiver(&m->queue);
    return 0;
}

int GroupObjects_Initialize(struct GroupObjects *m, int id)
{
    struct GroupPiece *p;
    int i;
    m->active = 0;
    m->actorId = 0;
    m->actor = 0;
    m->animation = ScriptProc_Start(0x922E, 0x5BB7);
    {
        void *first = gUnk_030042E4 + 0x5A60;
        void *second = gUnk_030042E4 + 0x5AA0;
        Palette_InterpolateBanks(m->palette, first, second, 64, 6);
    }
    p = m->pieces;
    for (i = 0; i < 16; i++, p++)
        BoardPiece_Initialize(m, p, i);
    SceneMessages_RegisterReceiver(&m->queue, id, 10);
    return 0;
}

void *GroupObjects_CreateTask(void *arg)
{
    void *obj;
    obj = Task_Create(11, 0xD08);
    if (obj != 0) {
        Task_SetCallbacks(obj, GroupObjects_Update, GroupObjects_Destroy);
        if (GroupObjects_Initialize(obj, (int)arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
