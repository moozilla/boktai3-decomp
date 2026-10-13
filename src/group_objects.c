#include "group_objects.h"

// MATCHING_CONCESSION: register-pinning
int BoardPiece_Initialize(struct GroupObjects *owner, struct GroupPiece *p, int index)
{
    struct GroupResource *resource;
    struct GroupSprite *sprite;
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
        AnimData_SelectTiles(r, 0x2D3);
    }
    resource->palette = owner->palette;
    sprite = &p->sprite;
    Sprite_Init(sprite, resource, 1);
    sprite->mode = 2;
    {
        register struct GroupVec *position asm("r5") = &p->sprite.position;
        {
            u32 zero = 0;
            {
                register u32 *src asm("r0") = &zero;
                struct GroupVec *dst = position;
                asm("" : : "r"(src), "r"(dst));
                CpuSet(src, dst, 0x05000002);
            }
        }
        Shadow_Initialize(p->transform, position, 0);
    }
    ActorShadow_Disable(p->transform);
}

u32 sub_0800FA40(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    sub_08013684(p->transform);
    Sprite_Remove(&p->sprite);
}

static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}

void sub_08220C8C(void *, void *, int, u8, u8);
void sub_0800FA5C(struct GroupObjects *m, struct GroupPiece *p, int index)
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
    sub_08220C8C(&p->anim, m->animation, (u16)index, left, right);
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

INCLUDE_ASM("asm/nonmatching", sub_0800FC28);
INCLUDE_ASM("asm/nonmatching", sub_0800FE98);
INCLUDE_ASM("asm/nonmatching", sub_0801010C);
INCLUDE_ASM("asm/nonmatching", sub_08010298);

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
            sub_08020D54(queue, cmd);
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

static inline void addVec(struct GroupVec *d, struct GroupVec *s)
{
    struct GroupVec *dst = d;
    struct GroupVec *src = s;
    {
        u32 value = (u16)s->x;
        value = value + (u16)d->x;
        d->x = value;
    }
    dst->y += src->y;
    dst->z += src->z;
}

// MATCHING_CONCESSION: register-pinning

void BoardPiece_Launch(struct GroupObjects *m, struct GroupPiece *p, int delay)
{
    struct GroupVec delta;
    if (p->active)
        p->active = 0;
    polar(&delta, p->direction, p->duration * 2);
    addPosition(&p->position, &delta);
    sub_0800FA5C(m, p, 0);
    if (p->timer >= delay + 2) {
        p->state = 1;
        p->active = 1;
        p->timer = 0;
    }
    p->timer++;
}

INCLUDE_ASM("asm/nonmatching", sub_08010514);
INCLUDE_ASM("asm/nonmatching", sub_0801068C);
INCLUDE_ASM("asm/nonmatching", sub_08010860);

void sub_08010A34(struct GroupObjects *m, struct GroupPiece *p)
{
    if (p->active != 0) {
        p->active = 0;
        p->sprite.flags |= 1;
        ActorShadow_Disable(p->transform);
    }
}

#ifdef NONMATCHING
void sub_08010A58(struct GroupObjects *m, struct GroupPiece *p, int index)
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
        p->direction = sub_082215E4(target->x - position->x, target->z - position->z);
        table = gUnk_0203B400;
        p->direction += (randomWith(table) & 31) - 16;
        p->speed = (randomWith(table) & 15) + 16;
    }
    if (p->speed == 0) {
        const u16 *table;
        p->direction = sub_082215E4(target->x - position->x, target->z - position->z);
        table = gUnk_0203B400;
        p->direction += (randomWith(table) & 15) - 8;
        p->speed = (randomWith(table) & 15) + 16;
    } else
        p->speed--;
    dx = p->target.x - p->position.x;
    dz = target->z - position->z;
    if (dx * dx + dz * dz <= 0xFFF) {
        p->state = 4;
        p->active = 1;
        p->timer = 0;
    } else {
        polar(&delta, p->direction, p->duration);
        addVec(position, &delta);
        sub_0800FA5C(m, p, 0);
        p->timer++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08010A58);
#endif

#ifdef NONMATCHING
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
        u32 zero;
        p->active = 0;
        p->anchor = p->position;
        p->direction = randomValue();
        zero = 0;
        CpuSet(&zero, offset, 0x05000002);
    }
    sub_082216F8(&p->anchor, &p->anchor, &p->target, index + 8, 5);
    if (p->speed == 0) {
        const u16 *table;
        p->direction = sub_082215E4(-p->offset.x, -p->offset.z);
        table = gUnk_0203B400;
        p->direction += (randomWith(table) & 15) - 7;
        p->speed = (randomWith(table) & 7) + 8;
    } else
        p->speed--;
    polar(&delta, p->direction, p->duration);
    addVec(&p->offset, &delta);
    p->position.x = p->anchor.x + p->offset.x;
    p->position.z = p->anchor.z + p->offset.z;
    sub_0800FA5C(m, p, 0);
    p->timer++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08010C10);
#endif
INCLUDE_ASM("asm/nonmatching", sub_08010D84);
INCLUDE_ASM("asm/nonmatching", sub_08010E44);

int GroupObjects_Update(struct GroupObjects *m)
{
    int i;
    struct GroupPiece *p;
    BoardPieces_DispatchCommands(m);
    if (m->active) {
        if (m->actorId)
            m->actor = sub_08225984(m->actorId);
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
            sub_08221110(m->palette, first, second, m->paletteMask, 6);
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
        sub_0800FA40(m, p, i);
    sub_08020D08(&m->queue);
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
        sub_08221110(m->palette, first, second, 64, 6);
    }
    p = m->pieces;
    for (i = 0; i < 16; i++, p++)
        BoardPiece_Initialize(m, p, i);
    SceneMessages_RegisterReceiver(&m->queue, id, 10);
    return 0;
}

void *sub_080111A8(void *arg)
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
