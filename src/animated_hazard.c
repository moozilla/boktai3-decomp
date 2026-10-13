#include "animated_hazard.h"

struct AnimatedHazardManager *sub_08012040(void)
{
    return gUnk_02000050;
}

void sub_0801204C(void *unused, struct AnimatedHazard *p, u32 v)
{
    u32 z = 0;
    p->state = v;
    p->timer = z;
    p->entered = 1;
}

s32 sub_08012058(void *unused, struct AnimatedHazard *p)
{
    s32 r;
    if (p->entered == 0) {
        r = 0;
    } else {
        p->entered = 0;
        r = 1;
    }
    return r;
}

void AnimatedHazard_Initialize(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct HazardExtent size, offset;
    int zero;
    u8 *sprite = (u8 *)&p->sprite, *effect;
    Sprite_Init(sprite, m->animData, 1);
    zero = 0;
    sprite[7] = 2;
    Anim_SetSequence(&p->anim, m->script, 0, 0, zero);
    effect = p->collider;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(effect, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(effect, 0, p);
    sub_0821FF58(effect, 100, 50, 0, 0x60000, 30);
    p->active = zero;
}

void sub_0801204C(void *, struct AnimatedHazard *, u32);

void sub_08012114(void *a, struct AnimatedHazard *p)
{
    p->sprite.flags |= 1;
    sub_0801204C(a, p, 0);
    p->active = 0;
}

void sub_08012134(void)
{
}

void AnimatedHazard_WaitForPlayer(void *m, struct AnimatedHazard *p)
{
    if (gUnk_02000710->unk876 > 0)
        p->sprite.flags &= ~1;
    else
        p->sprite.flags |= 1;
    if (++p->timer > p->duration)
        sub_0801204C(m, p, 3);
    else {
        struct SceneVec *player = sub_0815F53C(0);
        struct SceneVec *pos = &p->sprite.position;
        int x = player->x - pos->x, z = player->z - pos->z;
        if (x * x + z * z < 14400)
            sub_0801204C(m, p, 3);
    }
}

s32 sub_08012058(void *, struct AnimatedHazard *);

void AnimatedHazard_EmitWarning(void *m, struct AnimatedHazard *p)
{
    sub_08012058(m, p);
    if (p->timer < 60) {
        switch (p->timer) {
            case 0:
            case 24:
            case 48:
                AnimatedParticles_EmitOffset(&p->sprite.position, 1, 3, 0x3000, 64, 8, 256, 256);
                break;
        }
    }
    if (++p->timer > 89)
        sub_0801204C(m, p, 3);
}

static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}

// MATCHING_CONCESSION: register-pinning
void AnimatedHazard_AnimateAttack(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct HazardSprite *o;
    struct HazardAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    int done;
    if (sub_08012058(m, p)) {
        p->sprite.flags &= ~1;
        Anim_SetSequence(&p->anim, m->script, 0, 0, 0);
        Sound_PlaySongChecked(0x34D);
    }
    o = &p->sprite;
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
    if (done)
        sub_0801204C(m, p, 4);
    Collider_Refresh(p->collider);
}

// MATCHING_CONCESSION: register-pinning
void AnimatedHazard_AnimateHold(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct HazardSprite *o;
    struct HazardAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    int done;
    if (sub_08012058(m, p)) {
        Anim_SetSequence(&p->anim, m->script, 1, 0, 0);
    }
    o = &p->sprite;
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
    if (p->timer >= p->hold)
        sub_0801204C(m, p, 5);
    Collider_Refresh(p->collider);
}

void sub_08012114(void *, struct AnimatedHazard *);

// MATCHING_CONCESSION: register-pinning
void AnimatedHazard_AnimateRetraction(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct HazardSprite *o;
    struct HazardAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    int done;
    if (sub_08012058(m, p)) {
        Anim_SetSequence(&p->anim, m->script, 2, 0, 0);
    }
    o = &p->sprite;
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
    if (done)
        sub_08012114(m, p);
}

extern const u32 gUnk_08604B6C[];

void sub_080125FC(void *a, struct AnimatedHazard *p)
{
    sub_08249240(a, p, (void *)gUnk_08604B6C[p->state]);
    p->timer += 1;
}

void sub_08012620(void *unused, struct AnimatedHazard *p)
{
    Sprite_Remove(&p->sprite);
}

void sub_080125FC(void *, struct AnimatedHazard *);

s32 sub_08012630(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e = s->hazards;
    s32 i;
    for (i = 7; i >= 0; i--, e++) {
        if (e->active != 0)
            sub_080125FC(s, e);
    }
    return 0;
}

void sub_08012620(void *, struct AnimatedHazard *);

u32 sub_08012658(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e = s->hazards;
    s32 i;
    for (i = 7; i >= 0; i--, e++) {
        sub_08012620(s, e);
    }
    return gUnk_02000050 = 0;
}

void AnimatedHazard_Initialize(struct AnimatedHazardManager *, struct AnimatedHazard *);

s32 sub_08012684(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e;
    s32 i;
    gUnk_02000050 = s;
    s->script = ScriptProc_Start(0x922E, 0xE92A);
    if (AnimData_Load(s->animData, 0xCD6C) == 0)
        return -1;
    e = s->hazards;
    for (i = 7; i >= 0; i--, e++) {
        AnimatedHazard_Initialize(s, e);
    }
    return 0;
}

s32 sub_08012684(struct AnimatedHazardManager *);

s32 sub_08012630(struct AnimatedHazardManager *);

u32 sub_08012658(struct AnimatedHazardManager *);

void *sub_080126DC(void)
{
    struct AnimatedHazardManager *p;
    if (gUnk_02000050 != 0)
        return gUnk_02000050;
    p = Task_Create(8, 0x538);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))sub_08012630, (void (*)(void *))sub_08012658);
        if (sub_08012684(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

struct AnimatedHazard *sub_08012728(struct AnimatedHazardManager *p)
{
    struct AnimatedHazard *q = p->hazards;
    s32 i = 0;
    while (i <= 7) {
        if (q->active == 0) {
            return q;
        }
        i++;
        q++;
    }
    return 0;
}

struct AnimatedHazardManager *sub_08012040(void);

void *sub_080126DC(void);

struct AnimatedHazard *sub_08012728(struct AnimatedHazardManager *);

static inline u8 *cell(int i)
{
    u8 *p = sub_0821D64C(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}

int AnimatedHazard_Create(struct SceneVec *source, int damage, int hold)
{
    struct AnimatedHazardManager *m = sub_08012040();
    struct AnimatedHazard *p;
    struct SceneVec *pos;
    void *effect;
    u8 *c;
    int height, slope;
    int zero;
    if (!m) {
        m = sub_080126DC();
        if (!m)
            return -1;
    }
    p = sub_08012728(m);
    if (!p)
        return -1;
    p->sprite.position = *source;
    {
        struct SceneVec *ground = &p->sprite.position;
        int x = ground->x >> 8, z = ground->z >> 8, index;
        pos = ground;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else
            index = gUnk_030052F4->rows[z] + x;
        c = cell(index);
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)ground->z;
                break;
            case 2:
                height -= (u8)ground->x;
                break;
        }
    }
    {
        s16 *h = &p->sprite.position.y;
        zero = 0;
        *h = height;
    }
    effect = p->collider;
    Collider_SetPosition(effect, pos, 0);
    sub_0821FF50(effect, (u16)damage, 50);
    sub_0801204C(m, p, 2);
    p->timer = zero;
    p->hold = hold;
    p->active = 1;
    return 0;
}

// MATCHING_CONCESSION: register-pinning
int AnimatedHazard_CreateProximity(struct SceneVec *source, int damage, int delay, int hold)
{
    struct AnimatedHazardManager *m = sub_08012040();
    struct AnimatedHazard *p;
    struct SceneVec *pos;
    void *effect;
    u8 *c;
    int height, slope;
    int zero;
    if (!m) {
        m = sub_080126DC();
        if (!m)
            return -1;
    }
    p = sub_08012728(m);
    if (!p)
        return -1;
    p->sprite.position = *source;
    {
        struct SceneVec *ground = &p->sprite.position;
        int x = ground->x >> 8, z = ground->z >> 8, index;
        pos = ground;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else
            index = gUnk_030052F4->rows[z] + x;
        c = cell(index);
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)ground->z;
                break;
            case 2:
                height -= (u8)ground->x;
                break;
        }
    }
    {
        s16 *h = &p->sprite.position.y;
        zero = 0;
        *h = height;
    }
    effect = p->collider;
    Collider_SetPosition(effect, pos, 0);
    {
        register int d asm("r9") = damage;
        sub_0821FF58(effect, d, 50, 0, 8, 30);
    }
    sub_0801204C(m, p, 1);
    Anim_SetSequence(&p->anim, m->script, 0, 0, zero);
    {
        struct HazardSprite *o = &p->sprite;
        struct HazardAnim *a;
        const u16 *entry;
        register u16 n asm("r0");
        int done;
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
    }
    AnimData_SelectTiles(m->animData, 0x33F);
    p->timer = 0;
    p->hold = hold;
    asm("" : "+r"(delay));
    p->duration = delay;
    p->active = 1;
    return 0;
}

s32 sub_08012A88(void)
{
    struct AnimatedHazardManager *s = sub_08012040();
    struct AnimatedHazard *e;
    s32 i;
    if (s == 0) {
        s = sub_080126DC();
        if (s == 0)
            return -1;
    }
    e = s->hazards;
    for (i = 7; i >= 0; i--) {
        if (e->active != 0 && e->state <= 2)
            sub_0801204C(s, e, 3);
        e++;
    }
    return 0;
}
