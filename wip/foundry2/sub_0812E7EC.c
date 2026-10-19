#include "global.h"

struct Ent {
    u8 pad0[6];
    u8 type;
    u8 pad7;
    u16 a;
    u16 b;
    u16 c;
    s16 d;
};

struct Ent2 {
    u8 pad[0xc];
    u32 w0c;
    u32 w10;
    u8 pad2[8];
    u16 h1c;
    u16 h1e;
    u16 h20;
    u16 h22;
};

struct Glob {
    u8 pad[0x24];
    u8 idx;
};

extern struct Glob *gUnk_0200047C;
extern void sub_0807FC90();
extern void sub_08078BB4(void *, s32, s32, s32, u32);
extern void sub_08020D54(void *, void *);
extern u32 Trigger_ConsumePending(void *latch, u32 expected);
extern void SceneActorRegistry_SendToActorId(u8 a, void *data, u16 c);
extern void SceneActorAttachment_SelectOffsets(u8 category, u8 kind, s32 *yOffset, s32 *zOffset);
extern void *ActorAttachment_CreateValueSpriteSlot(u32, void *, void *, s32, s32, u32, s16 *, s32, u32, void *, void *);
extern void *ActorAttachment_CreatePartsSlotWithCount(u8, void *, void *, s32, u32, s16 *, s32, u32, void *, u32);
extern void *ActorAttachment_UpdateFromDescriptor();
extern u32 ActorAttachment_ReleaseResource();
extern u32 ActorAttachment_ReleasePartsResource();

static inline void startAct(u8 *p, u32 mode, u32 zero, u32 arg)
{
    *(u8 *)(p + 0x2a8) = mode;
    *(u8 *)(p + 0x2a9) = zero;
    *(u8 *)(p + 0x2aa) = arg;
    *(u32 *)(p + 0x2ac) = zero;
    *(u8 *)(p + 0x2b1) = 1;
    *(u8 *)(p + 0x2b2) = 1;
    *(u8 *)(p + 0x2b0) = zero;
    *(u8 *)(p + 0x2b5) = 0x10;
    *(void **)(p + 0x21c) = sub_0807FC90;
    *(u16 *)(p + 0x2c8) = zero;
    *(u32 *)(p + 0x114) &= ~1;
    *(u32 *)(p + 0x104) = zero;
}
#define START(arg) startAct(p, 2, 0, arg)

static inline u8 hasFlag(u8 *p)
{
    if (*(u16 *)(p + 0x2c0) & 0x100)
        return 1;
    return 0;
}

static inline u8 isOne(u8 *p)
{
    if (*(s32 *)p == 1)
        return 1;
    return 0;
}

u32 sub_0812E7EC(u8 *p)
{
    u32 slot;
    u32 id;
    struct Ent *e;
    u8 *base;
    u8 *q;
    u8 *tq;
    s32 i;
    u16 msg[3];
    s32 yo, zo;
    s32 dd;
    s32 dir;

    if (!hasFlag(p))
        return 0;
    if (isOne(p))
        base = p + 0x3d4;
    else
        base = 0;
    slot = 0;
    q = p + 0xc;
    tq = p + 0xc;
    id = p[0x98];
    for (i = 0; i < p[0x12 + gUnk_0200047C->idx]; i++) {
        e = *(struct Ent **)(q + 8 + i * 4 + gUnk_0200047C->idx * 16);
        switch (e->type) {
        case 0:
            if (id == 0x12) {
                *(u16 *)(p + 0x2fc) = e->a;
                *(u16 *)(p + 0x2fe) = e->b;
                *(u16 *)(p + 0x300) = e->c;
                START(3);
            } else {
                struct Ent2 *t = *(struct Ent2 **)(base + 0x1fc);
                t->h1c = e->a;
                t = *(struct Ent2 **)(base + 0x1fc);
                t->h1e = e->b;
                t = *(struct Ent2 **)(base + 0x1fc);
                t->h20 = e->c;
                if (e->d)
                    *(u16 *)(p + 0x2d4) |= 0x200;
                else
                    *(u16 *)(p + 0x2d4) &= ~0x200;
                t = *(struct Ent2 **)(base + 0x1fc);
                {
                    u32 w1 = *(u32 *)((u8 *)t + 0x1c);
                    u32 w2 = *(u32 *)((u8 *)t + 0x20);
                    t->w0c = w1;
                    t->w10 = w2;
                }
                START(3);
            }
            break;
        case 1:
            dir = p[0x59];
            if (dir < 0)
                dd = -((-dir) >> 5);
            else
                dd = dir >> 5;
            dd = (dd + 3) & 7;
            dd = e->a - dd;
            if (dd < 0)
                dd = -dd;
            if (id == 0x17) {
                if (dd == 0)
                    goto consume;
                p[0x3cd] = ((e->a + 5) & 7) << 5;
                START(4);
            } else if (dd > 1) {
                p[0x3cd] = e->a;
                START(4);
            } else {
            consume:
                Trigger_ConsumePending(tq, 1);
            }
            break;
        case 2:
            p[0x59] = ((e->a + 5) & 7) << 5;
            START(2);
            break;
        case 4:
            START(5);
            break;
        case 8:
            p[0x59] = ((e->a + 5) & 7) << 5;
            START(6);
            break;
        case 9:
            p[0x59] = ((e->a + 5) & 7) << 5;
            START(7);
            break;
        case 10:
            p[0x59] = ((e->a + 5) & 7) << 5;
            START(8);
            break;
        case 11:
            START(9);
            break;
        case 5:
            msg[0] = 6;
            goto send;
        case 6:
            msg[0] = 6;
            msg[1] = e->a;
            msg[2] = e->b;
            goto send;
        case 7:
            if (id == 0x17 || id == 0x12)
                msg[0] = 7;
            else
                msg[0] = 8;
            msg[1] = e->a;
        send:
            SceneActorRegistry_SendToActorId(id, msg, *(u32 *)(p + 0x50));
            break;
        case 3:
            switch (e->a) {
            case 0:
                sub_08078BB4(p, i, 0, 1, id);
                break;
            case 1:
            case 2:
                sub_08078BB4(p, i, 0, 0, id);
                break;
            case 3:
                sub_08078BB4(p, i, 0, 2, id);
                break;
            case 4:
                *(s16 *)(p + 0xaa) = e->b;
                *(s16 *)(p + 0xa8) = e->c;
                slot = 0;
                SceneActorAttachment_SelectOffsets(id, 1, &yo, &zo);
                slot = (u32)ActorAttachment_CreateValueSpriteSlot(0, p + 0x5c, *(void **)(p + 0x50), yo, zo, id,
                                                                   (s16 *)(p + 0xaa), *(s16 *)(p + 0xa8), slot, p, p + 0x50);
                break;
            case 5:
                *(s16 *)(p + 0xaa) = e->b;
                *(s16 *)(p + 0xa8) = e->c;
                SceneActorAttachment_SelectOffsets(id, 3, &yo, &zo);
                slot = (u32)ActorAttachment_CreatePartsSlotWithCount(7, p + 0x5c, *(void **)(p + 0x50), zo, 0,
                                                                      (s16 *)(p + 0xaa), *(s16 *)(p + 0xa8), e->d, p + 0x50, (u32)p);
                break;
            case 6:
                SceneActorAttachment_SelectOffsets(id, 1, &yo, &zo);
                slot = (u32)ActorAttachment_UpdateFromDescriptor(slot, p + 0x50, 5, 0, 0, 0, zo, yo);
                break;
            case 7:
                SceneActorAttachment_SelectOffsets(id, 1, &yo, &zo);
                slot = (u32)ActorAttachment_UpdateFromDescriptor(slot, p + 0x50, 4, 0, 0, 0, zo, yo);
                break;
            case 8: {
                u32 *r = (u32 *)(base + 0xa0);
                *r = ActorAttachment_ReleaseResource(*r, 0, *(u32 *)(p + 0x50));
                *r = ActorAttachment_ReleaseResource(*r, 1, *(u32 *)(p + 0x50));
                *r = ActorAttachment_ReleaseResource(*r, 2, *(u32 *)(p + 0x50));
                *r = ActorAttachment_ReleaseResource(*r, 4, *(u32 *)(p + 0x50));
                *r = ActorAttachment_ReleaseResource(*r, 5, *(u32 *)(p + 0x50));
                *r = ActorAttachment_ReleasePartsResource(*r, 3, *(u32 *)(p + 0x50));
                break;
            }
            }
            Trigger_ConsumePending(tq, 1);
            break;
        default:
            break;
        }
        sub_08020D54(q, e);
    }
    return 1;
}
