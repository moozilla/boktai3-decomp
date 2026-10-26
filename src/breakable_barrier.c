// MATCHING_CONCESSION: register-pinning
#include "breakable_barrier.h"

void BreakableBarrier_SetState(struct BreakableBarrier *s, u8 v)
{
    s->state = v;
    s->timer = 0;
    s->entered = 1;
}

u32 BreakableBarrier_ConsumeEntered(struct BreakableBarrier *p)
{
    u8 *q = &p->entered;
    u32 r;
    if (*q == 0) {
        r = 0;
    } else {
        *q = 0;
        r = 1;
    }
    return r;
}

// MATCHING_CONCESSION: register-pinning
struct BreakableBarrier *BreakableBarrier_SetPosition(struct BreakableBarrier *input,
                                                      struct SceneVec *position, int orientation)
{
    register struct BreakableBarrier *temp asm("r3") = input;
    register struct BreakableBarrier *p asm("r4");
    register u32 x asm("r5");
    register u32 y asm("r3");
    register u32 value asm("r1");
    asm("" : : "r"(temp));
    p = temp;
    x = (u16)position->x;
    p->sprite.position.x = x;
    y = (u16)position->y;
    p->sprite.position.y = y;
    value = (u16)position->z;
    p->sprite.position.z = value;
    if (orientation) {
        value -= 128;
        p->sprite.position.z = value;
    } else {
        value = x - 128;
        p->sprite.position.x = value;
    }
    value = (u16)p->sprite.position.x + 60;
    p->sprite.position.x = value;
    value = (u16)p->sprite.position.z + 60;
    p->sprite.position.z = value;
    value = (u16)p->sprite.position.y + 20;
    p->sprite.position.y = value;
    return input;
}

void BreakableBarrier_OnColliderHit(u32 a, u32 b, struct BreakableBarrier *p)
{
    if (p) {
        p->hit = 1;
    }
}

struct BreakableBarrier *BreakableBarrier_SetPosition(struct BreakableBarrier *, struct SceneVec *,
                                                      int);

void BreakableBarrier_OnColliderHit(u32, u32, struct BreakableBarrier *);

void BreakableBarrier_SetState(struct BreakableBarrier *, u8);

int BreakableBarrier_Initialize(struct BreakableBarrierManager *m, struct BreakableBarrier *p,
                                unsigned i)
{
    struct BarrierExtent size, offset;
    int active = 0;
    void *resource;
    struct SceneCollider *c;
    if (i < m->count)
        active = 1;
    resource = p->resource;
    if (!AnimData_Load(resource, 0x9929))
        return -1;
    Sprite_Init(p, resource, 0);
    if (!active)
        p->sprite.flags |= 1;
    if (m->direction[i])
        p->sprite.flags |= 4;
    p->sprite.priority = 2;
    BreakableBarrier_SetPosition(p, &m->positions[i], m->direction[i]);
    c = &p->collider;
    if (m->direction[i]) {
        size.x = 128;
        size.y = 128;
        size.z = 110;
        offset.x = 32;
        offset.y = 128;
        offset.z = 0;
    } else {
        size.x = 110;
        size.y = 128;
        size.z = 128;
        offset.x = 0;
        offset.y = 128;
        offset.z = 32;
    }
    Collider_Init(c, 0, 0x4001, 0, 16, &size, &offset);
    if (!active)
        c->state |= 4;
    Collider_SetPosition(c, &p->sprite.position, 0);
    Collider_SetCallback(c, BreakableBarrier_OnColliderHit, p);
    Collider_SetBodyProperties(c, 0, 0, 0);
    Collider_Refresh(c);
    if (active)
        BreakableBarrier_SetState(p, 1);
    else
        BreakableBarrier_SetState(p, 0);
    p->health = m->health;
    return 0;
}

u32 BreakableBarrier_Destroy(u32 a, struct BreakableBarrier *p)
{
    Collider_Remove(&p->collider);
    Sprite_Remove(p);
    return 0;
}

static inline void disable(struct SceneCollider *c)
{
    c->state |= 4;
}

u32 BreakableBarrier_ConsumeEntered(struct BreakableBarrier *);

void BreakableBarrier_Hide(void *m, struct BreakableBarrier *p)
{
    if (BreakableBarrier_ConsumeEntered(p)) {
        p->sprite.flags |= 1;
        disable(&p->collider);
    }
}

static inline void enable(struct SceneCollider *c)
{
    u32 mask = ~4;
    c->state = mask & c->state;
}

// MATCHING_CONCESSION: register-pinning
void BreakableBarrier_Update(struct BreakableBarrierManager *m, struct BreakableBarrier *p)
{
    struct BarrierExtent size;
    if (BreakableBarrier_ConsumeEntered(p)) {
        p->sprite.flags &= ~1;
        AnimData_SelectPaletteBank(p->resource, 136);
    }
    p->frame++;
    if (p->frame == 7)
        p->sprite.frame = 1;
    else if (p->frame == 14) {
        p->sprite.frame = 0;
        p->frame = 0;
    }
    if (p->hit) {
        p->hit = 0;
        p->flash = 10;
        p->health--;
        if ((s16)p->health <= 0) {
            int angle, speed;
            int count;
            const u16 *table;
            u32 *indexPtr;
            u32 mask;
            u32 smallMask;
            register u32 index asm("r0");
            size.x = 164;
            size.y = 16;
            size.z = 164;
            table = gUnk_0203B400;
            indexPtr = &gRandomIndex;
            index = *indexPtr;
            index++;
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
            SparkParticle_Spawn(&p->sprite.position, 3, &size, angle, count, speed, 2, 2);
            {
                u32 *ptr = indexPtr;
                index = (*ptr + 1) & mask;
            }
            angle = (u8)table[index];
            index = (index + 1) & mask;
            count = (table[index] & smallMask) + 2;
            index = (index + 1) & mask;
            *indexPtr = index;
            speed = (table[index] & smallMask) + 20;
            SparkParticle_Spawn(&p->sprite.position, 3, &size, angle, count, speed, 2, 2);
            BreakableBarrier_SetState(p, 2);
            return;
        } else {
            sub_08012B34(m, p);
            sub_08012B34(m, p);
            sub_08012B34(m, p);
            Sound_PlaySongChecked(232);
            AnimData_SelectPaletteBank(p->resource, 402);
        }
    }
    if (p->flash) {
        p->flash--;
        if (!p->flash) {
            AnimData_SelectPaletteBank(p->resource, 136);
            enable(&p->collider);
        } else
            disable(&p->collider);
    }
}

void Vampire_DecrementBarrierCount(void);
void Palette_StartSlide(s32, s32, s32);

void sub_0801310C(struct BreakableBarrierManager *m, struct BreakableBarrier *p)
{
    u32 index_3;
    u32 index_2;
    struct BarrierExtent size;
    if (BreakableBarrier_ConsumeEntered(p)) {
        disable(&p->collider);
    }
    if ((p->timer & 3) == 0) {
        s32 angle, count, speed;
        const u16 *table;
        u32 *indexPtr;
        u32 index;
        u32 mask;
        size.x = 164;
        size.y = 16;
        size.z = 164;
        table = gUnk_0203B400;
        indexPtr = &gRandomIndex;
        index = *indexPtr;
        index++;
        mask = 1023;
        index &= mask;
        angle = (u8)table[index];
        index_2 = (index + 1) & mask;
        {
            u32 v = table[index_2];
            u32 m3 = 3;
            v &= m3;
            count = v + 2;
        }
        index_3 = (index_2 + 1) & mask;
        *indexPtr = index_3;
        {
            u32 v = table[index_3];
            u32 m3 = 3;
            v &= m3;
            speed = v + 20;
        }
        SparkParticle_Spawn(&p->sprite.position, 1, &size, angle, count, speed, 2, 2);
        sub_08012B34(m, p);
        Sound_PlaySongChecked(0x339);
    }
    if (p->timer > 63) {
        Vampire_DecrementBarrierCount();
        if (m->remaining != 0) {
            s32 step;
            m->remaining--;
            step = (4 - m->remaining) << 4;
            Palette_StartSlide(step - 16, step, 64);
            if (m->remaining == 0 && m->script != 0) {
                Script_Call(m->script, 0);
                m->script = 0;
            }
        }
        BreakableBarrier_SetState(p, 0);
    }
}

extern const u32 gUnk_08604B84[];

u32 BreakableBarrier_UpdateSprite(void *a, struct BreakableBarrier *p)
{
    sub_08249240(a, p, (void *)gUnk_08604B84[p->state]);
    p->timer += 1;
    return 0;
}

u32 BreakableBarrier_UpdateSprite(void *, struct BreakableBarrier *);

u32 BreakableBarriers_Update(struct BreakableBarrierManager *p)
{
    struct BreakableBarrier *q = p->barriers;
    s32 i = 3;
    do {
        BreakableBarrier_UpdateSprite(p, q);
        i--;
        q++;
    } while (i >= 0);
    return 0;
}

u32 BreakableBarrier_Destroy(u32, struct BreakableBarrier *);

u32 BreakableBarriers_Destroy(struct BreakableBarrierManager *p)
{
    struct BreakableBarrier *q = p->barriers;
    s32 i = 3;
    do {
        BreakableBarrier_Destroy((u32)p, q);
        i--;
        q++;
    } while (i >= 0);
    return 0;
}

int BreakableBarrier_Initialize(struct BreakableBarrierManager *, struct BreakableBarrier *,
                                unsigned);

int BreakableBarriers_Initialize(struct BreakableBarrierManager *m)
{
    int i;
    struct BreakableBarrier *p;
    m->count = 0;
    m->remaining = 0;
    m->health = Script_GetKeywordValue('l', 5);
    if (Script_SeekToKeyword('p')) {
        for (i = 0; i < 4 && Script_GetPc(); i++) {
            m->positions[i].x = Script_GetValue();
            m->positions[i].y = Script_GetValue();
            m->positions[i].z = Script_GetValue();
        }
        m->count = i;
        m->remaining = i;
    }
    if (Script_SeekToKeyword('d')) {
        u32 *d;
        i = 0;
        if (i < m->count) {
            d = m->direction;
            do {
                *d++ = Script_GetValue();
                i++;
            } while (i < m->count);
        }
    }
    p = m->barriers;
    for (i = 0; i < 4; i++, p++)
        if (BreakableBarrier_Initialize(m, p, i) < 0)
            return -1;
    m->script = Script_GetKeywordValue('c', 0);
    gBgFadeColor = 0x1084;
    gBgFadeBankMask = 0x1FFF;
    {
        int v = 64 - (int)(m->remaining * 40) / m->count;
        if (v > 63)
            v = 64;
        else if (v < 0)
            v = 0;
        m->blend = v;
    }
    return 0;
}

int BreakableBarriers_Initialize(struct BreakableBarrierManager *);

u32 BreakableBarriers_Update(struct BreakableBarrierManager *);

u32 BreakableBarriers_Destroy(struct BreakableBarrierManager *);

void *BatCurtain_Create(void)
{
    void *obj;
    obj = Task_Create(8, 0x2FC);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void (*)(void *))BreakableBarriers_Update,
                          (void (*)(void *))BreakableBarriers_Destroy);
        if (BreakableBarriers_Initialize(obj) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
