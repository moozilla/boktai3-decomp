#include "global.h"
#include "scene_actor_reaction.h"

struct G {
    u8 unknown0[0x40];
    u16 flags414;
    u8 unknown42[4];
    u16 field46;
    u32 field48;
    u8 unknown4A[0x74 - 0x4A];
    struct {
        u8 unknown0[8];
        const u16 *point;
    } route;
    u8 unknown80[0xC];
    u32 savedControl;
    u8 savedState, savedPhase, savedBehavior, savedSubstate;
    u8 state, phase, behavior, unknown97;
    s32 threshold;
    const void *control;
    u8 unknownA0[0x1FC - 0xA0];
    void *context;
    struct ContactPosition *destination;
    u8 field204, field205, field206;
};
struct W {
    u8 unknown0[8];
    u8 state, phase, behavior, unknownB;
    s32 threshold;
    const void *control;
};
struct Tgt {
    u8 unknown0[8];
    u8 index;
    u8 unknown9[3];
    u16 x;
    u16 unknownE;
    u16 z;
};
struct Pl {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x1C];
    struct {
        u8 unknown0[6];
        u16 cell;
    } *data;
};
extern struct Pl *gUnk_02000580[4];

u8 sub_0812F3CC(void *);
void sub_0807E334(void *);
s32 Mod(s32, s32);
s32 Div(s32, s32);

static inline void SetAnim(struct SceneActorRegistryResetView *actor, u32 st, u32 ph, u32 b, u32 cnt, u32 one)
{
    actor->state2A8 = st;
    actor->phase2A9 = ph;
    actor->behavior2AA = b;
    actor->counter2AC = cnt;
    actor->field2B1 = one;
}

static inline u8 TakeFlag(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline u32 CellOf(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

static inline u32 Height(struct ContactPosition *pos, s32 x, s32 z)
{
    u32 cell, shape, height;
    u8 *data;
    u32 flags;
    cell = CellOf(x, z);
    data = sub_0821D64C(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        data = (u8 *)((u32)map->cells + offset);
    }
    flags = *data;
    shape = flags >> 4;
    height = (flags & 15) << 8;
    switch (shape) {
        case 1:
            height -= *(u8 *)&pos->z;
            break;
        case 2:
            height -= *(u8 *)&pos->x;
            break;
        default:
            break;
    }
    return height;
}

static inline void ClrH(u16 *f, u32 mask)
{
    *f = *f & mask;
}

static inline void ClrW(u32 *f, u32 mask)
{
    *f &= mask;
}

static inline u32 Tst(u32 *a, u32 m)
{
    return *a & m;
}

static inline u8 Bit4(u16 *f)
{
    if (*f & 4)
        return 1;
    return 0;
}

static inline void SetCtl(struct SceneActorRegistryResetView *actor, u32 kind, void *control, u32 one, u32 zb, u32 zw)
{
    actor->field2B2 = one;
    actor->field2B0 = zb;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zw;
    ClrW(&actor->flags114, ~1);
    actor->word104 = zw;
}

static inline u16 TstH(u32 m, u16 *a)
{
    return *a & m;
}

static inline u8 EngineFlag(void)
{
    if (gUnk_030053F4 & 0x4800)
        return 1;
    return 0;
}

static inline void Join(struct SceneActorRegistryResetView *actor, struct G *grp)
{
    u8 dir = actor->direction;
    if (!(actor->flags2D4 & 0x400) && !Bit4(&grp->flags414)) {
        u32 c = actor->direction;
        u32 i = 0, v;
        do {
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
            v = (gUnk_0203B400[gUnk_03005308] & 3) << 6;
            if (v != c)
                break;
            i++;
        } while (i <= 3);
        if (i > 3)
            v = c;
        grp->field204 = v;
        grp->savedControl = (u32)actor->target;
        grp->savedSubstate = actor->field2B5;
        grp->savedState = actor->state2A8;
        grp->savedPhase = actor->phase2A9;
        grp->savedBehavior = actor->behavior2AA;
        grp->field205 = dir;
        {
            u32 one = 1, z = 0, z4;
            SetCtl(actor, 0x1B, *(void **)((u8 *)grp + 0x21C), one, z, z);
            z4 = 0;
            SetAnim(actor, 3, one, z4, z, one);
        }
        grp->field206 = 6;
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
}

void sub_0812FA0C(struct SceneActorRegistryResetView *p)
{
    struct G *g = (struct G *)((u8 *)p + 0x3D4);

    if (TakeFlag(p)) {
        void *saved = p->savedTarget;
        SetCtl(p, 0, saved, 1, 0, 0);
        ClrW(&p->flags2CC, ~0x80);
        ClrH(&p->flags2D4, ~0x200);
    }
    if ((u8)sub_0824923C(p, *(void **)((u8 *)p + 0x270)))
        return;
    {
        u32 r6 = sub_0812F3CC(p);
        if (r6)
            return;
        if ((s32)p->counter2AC > 0x3C) {
            u32 zero = 0;
            void *rctl = SceneActorReaction_RestoreAfterDelay;
            u32 kind = 0xE;
            u32 one;
            ClrH(&g->flags414, ~6);
            one = 1;
            SetCtl(p, kind, rctl, one, zero, r6);
            {
                u16 *a = (u16 *)((u8 *)g + 0x46);
                u32 thr = *a;
                u32 ctl = g->field48;
                struct W *w = (struct W *)((u8 *)a + 0x46);
                w->threshold = thr;
                w->control = (void *)ctl;
                w->state = zero;
                w->phase = zero;
                w->behavior = 0xC;
            }
            SetAnim(p, 3, one, zero, r6, one);
            {
                struct SceneActorRegistryResetView *q = p;
    struct G *g = (struct G *)((u8 *)q + 0x3D4);
    u32 *dcp = &q->fieldDC;
    struct ContactPosition *dest = g->destination;
    const u16 *pt = g->route.point;
    u32 cell, idx;
    u16 hx = pt[0], hz = pt[1];
    dest->x = hx;
    dest->z = hz;
    dest->y = Height(dest, (s16)hx >> 8, (s16)hz >> 8);
    if (EngineFlag()) {
        struct Tgt *t = (struct Tgt *)*dcp;
        if (!t)
            goto fin;
        idx = t->index > 3 ? -1 : t->index;
        if ((s32)idx < 0)
            goto fin;
        cell = gUnk_02000580[idx]->data->cell;
    } else {
        struct Tgt *t = (struct Tgt *)*dcp;
        cell = CellOf((s16)t->x >> 8, (s16)t->z >> 8);
        idx = 0;
    }
    if (cell == q->terrainCell || Tst(&gUnk_02000580[idx]->flags, 0x20000)) {
        u32 zero = 0, one;
        u16 a = TstH(0x400, &q->flags2D4);
        if (a)
            return;
        ClrH(&g->flags414, ~4);
        {
            u16 b = TstH(0x80, &q->flags2D4);
            if (b) {
                one = 1;
                SetCtl(q, 0x1A, *(void **)&g->field48, one, zero, a);
                SetAnim(q, zero, zero, 0xC, a, one);
            } else {
                one = 1;
                SetCtl(q, 1, *(void **)((u8 *)q + 0x228), one, zero, b);
                SetAnim(q, zero, zero, 5, b, one);
                {
                    struct ContactPosition *d2 = g->destination;
                    u16 gx = pt[0], gz = pt[1];
                    d2->x = gx;
                    d2->z = gz;
                    d2->y = Height(d2, (s16)gx >> 8, (s16)gz >> 8);
                    g->destination->y = Height(g->destination, (s16)g->destination->x >> 8, (s16)g->destination->z >> 8);
                    g->destination->pad = 0;
                }
            }
        }
        Join(p, g);
    } else {
        struct ContactPosition buf;
        struct ContactPosition *bq = &buf;
        s32 w, mod, div;
        u8 *data;
        u32 ret;
        w = *(s16 *)((u8 *)gUnk_030052F4->cells + 4);
        mod = Mod(cell, w);
        div = Div(cell, w);
        bq->x = (mod << 8) + 0x80;
        data = sub_0821D64C(cell, 1);
        if (data)
            data += 4;
        else {
            struct EngineMap *map = gUnk_030052F4;
            u32 offset = cell * 4 + 12;
            data = (u8 *)((u32)map->cells + offset);
        }
        bq->y = (*data & 15) << 8;
        bq->z = (div << 8) + 0x80;
        {
            u16 *ff;
            if (!sub_0821F204(g->context, &q->position, bq)) {
                ff = &q->flags2D4;
                goto failed;
            }
            {
                s32 r = sub_0821F7A8(g->context, &q->position, bq);
                if (r) {
                    if (r != 2)
                        Memory_Clear(&q->motion, 8);
                    ret = 1;
                    goto done;
                }
            }
            ff = &q->flags2D4;
        failed:
            ClrH(ff, ~0x200);
            ret = 0;
        done:;
        }
        if (ret)
            return;
    fin:
        sub_0824923C(q, *(void **)((u8 *)g + 0x224));
    }
            }
        } else
            p->counter2AC++;
    }
}
