#include "global.h"
#include "scene_actor_palette.h"
#include "game_state.h"

extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern u32 gUnk_030053F4;
extern u16 gUnk_02000528;
extern u8 *gUnk_030042E4;
extern struct GameState *gUnk_02000710;
extern struct SceneActorRegistryNode *gUnk_020004AC;
void sub_08231500(void *, u32, void *, void *, void *, u32, u32);

static inline u8 Has16(u16 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}
static inline u8 Has32(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}
static inline u8 IsUnavailable(u16 *a, u32 *b, s16 *c)
{
    u32 m = 0x40;
    u32 n = 0x20000;
    if ((*a & m) || (*b & n) || *c <= 0)
        return 1;
    return 0;
}
static inline u8 IsZero(u32 v)
{
    if (v == 0)
        return 1;
    return 0;
}
static inline u8 IsOne(u16 v)
{
    if (v == 1)
        return 1;
    return 0;
}
static inline u8 Same(u16 b, u32 a)
{
    if (a == b)
        return 1;
    return 0;
}

static inline u32 Random(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(u16 *)(offset + (u32)gUnk_0203B400);
}

void sub_08070FA8(struct SceneActorRegistryManager *manager)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    u32 t;
    u16 *f2D4;
    u32 *f2D0;
    u32 *f2CC;
    node = gUnk_020004AC;
    if (!node)
        return;
    actor = node->actor;
    while (actor) {
        if (Has16(actor->flags2D6, 0x1000)) {
            void *ctx;
            node = node->next;
            ctx = actor->context;
            sub_0824923C(ctx, actor->destroy);
            Heap_Free(ctx);
            actor = node->actor;
            continue;
        }
        {
            void *fn = &actor->update;
            fn = *(void **)fn;
            sub_0824923C(actor->context, fn);
        }
        t = IsZero(*(u32 *)actor);
        f2D4 = &actor->flags2D4;
        if (t)
            goto next;
        t = Has32(gUnk_030053F4, 0x4800);
        f2D4 = &actor->flags2D4;
        if (t)
            goto next;
        {
            u32 m = 0x4000;
            if (*f2D4 & m)
                goto next;
        }
        f2D0 = &actor->flags2D0;
        {
            u32 m = 8;
            if (*f2D0 & m)
                goto next;
        }
        if (actor->enabled == 0)
            goto next;
        f2CC = &actor->flags2CC;
        {
            u32 m = 2;
            if (*f2CC & m)
                goto next;
        }
        if (!(*(u32 *)((u8 *)gUnk_02000710 + 0x868) & 2))
            goto next;
        if (!Same(actor->regionId, *(u32 *)((u8 *)gUnk_02000710 + 0x5A0)))
            goto next;
        if (IsUnavailable(f2D4, f2D0, &actor->health))
            goto next;
        if (*(s16 *)((u8 *)gUnk_02000710 + 0x876) <= 0)
            goto next;
        t = IsOne(gUnk_02000528);
        if (t)
            goto next;
        {
            actor->field178 = t;
            actor->field2B8 = t;
            if (actor->drawableKind == 0) {
                u32 *p = actor->drawable;
                u32 m = ~1;
                *p = *p & m;
            } else {
                u32 *p = (u32 *)((u8 *)actor->drawable + 0x20);
                u32 m = ~1;
                p[2] = p[2] & m;
            }
            actor->health = t;
            if (*f2D4 & 0x2000) {
                union SceneActorRegistryPackedPosition a, b, c;
                const struct ContactPosition **origin = &actor->effectOrigin;
                void *cfg = actor->effectConfig;
                s32 i = 2;
                do {
                    a.packed.x = (Random() & 15) - 7;
                    a.packed.y = (Random() & 31) - 16;
                    a.packed.z = (Random() & 15) - 7;
                    b.packed.x = 5;
                    b.packed.y = 10;
                    b.packed.z = 5;
                    c.packed.x = (*origin)->x;
                    c.packed.y = (*origin)->y + 128;
                    c.packed.z = (*origin)->z;
                    sub_08231500(cfg, 3, &c, &a, &b, 40, 40);
                } while (--i >= 0);
                actor->fieldAA = 0;
                actor->field2B8 = 0;
            }
            *f2D4 &= ~0x2000;
            *f2CC |= 0x1000;
        }
    next:
        {
            u32 m = 0x4000;
            if (*f2D4 & m)
                goto adv;
        }
        if (Has32(manager->flags, 0x2000)) {
            u8 cat = actor->category;
            if (cat == 10 || cat == 23) {
                if (!(actor->flags2CC & 0x02008000)) {
                    if (actor->drawableKind == 0)
                        ((u16 *)actor->drawable)[0x19] = actor->paletteBank;
                    else {
                        struct {
                            u8 pad[0x3A];
                            u16 v;
                        } *d = (void *)((u8 *)actor->drawable + 0x20);
                        d->v = actor->paletteOffset;
                    }
                }
                if ((u8)SceneActorPalette_ApplyTransition((void *)manager, actor)) {
                    t = Has32(manager->flags, 0x20000);
                    if (t) {
                        u32 idx;
                        manager->flags &= 0xFFFDDFFF;
                        actor->flags2D0 &= 0xFFDFFFFF;
                        idx = actor->basePaletteBank + actor->paletteOffset;
                        if (actor->drawableKind == 0)
                            AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, idx);
                        else
                            *(u8 **)((u8 *)actor->drawable + 0x68) = gUnk_030042E4 + idx * 32;
                    } else {
                        u32 m = actor->flags2D0 & 0x200000;
                        if (m) {
                            SceneActorPalette_BeginTransition(
                                actor->paletteBank, actor->basePaletteBank + actor->paletteOffset,
                                0, cat, t);
                            actor->flags2D0 &= 0xFFDFFFFF;
                        } else {
                            SceneActorPalette_BeginTransition(
                                actor->basePaletteBank + actor->paletteOffset, actor->paletteBank,
                                0, cat, m);
                            actor->flags2D0 |= 0x200000;
                        }
                    }
                }
            }
        }
    adv:
        node = node->next;
        actor = node->actor;
    }
}
