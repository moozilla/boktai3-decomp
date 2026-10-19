#include "global.h"

struct Cmd {
    u8 pad0[6];
    u8 kind;
    u8 pad7;
    s16 a;
    s16 b;
    s16 c;
    s16 d;
};
struct Recv {
    u8 pad0[6];
    u8 count[2];
    struct Cmd *pending[2][4];
};
struct Ctx {
    u8 pad0[0xC];
    u32 w0C, w10;
    u8 pad14[8];
    u16 h1C, h1E, h20;
};
struct Grp {
    u8 pad0[0xA0];
    u32 w0A0;
    u8 pad1[0x1FC - 0xA4];
    struct Ctx *ctx;
};
struct Obj {
    s32 mode;
    u8 pad4[8];
    struct Recv recv;
    u8 pad34[0x50 - 0x34];
    u32 handle;
    u8 pad54[5];
    u8 dir;
    u8 pad5A[2];
    s16 s5C;
    u8 pad5E[0x98 - 0x5E];
    u8 id;
    u8 pad99[0xA8 - 0x99];
    s16 s5A8, s5AA;
    u8 padAC[0x104 - 0xAC];
    u32 w104;
    u8 pad108[0x114 - 0x108];
    u32 flags114;
    u8 pad118[0x21C - 0x118];
    void *fn21C;
    u8 pad220[0x2A8 - 0x220];
    u8 b2A8, b2A9, b2AA, b2AB;
    u32 w2AC;
    u8 b2B0, b2B1, b2B2, b2B3, b2B4, b2B5;
    u8 pad2B6[0x2C0 - 0x2B6];
    u16 h2C0;
    u8 pad2C2[0x2C8 - 0x2C2];
    u16 h2C8;
    u8 pad2CA[0x2D4 - 0x2CA];
    u16 h2D4;
    u8 pad2D6[0x2FC - 0x2D6];
    u16 h2FC, h2FE, h300;
    u8 pad302[0x3CD - 0x302];
    u8 b3CD;
    u8 pad3CE[0x3D4 - 0x3CE];
};
extern struct { u8 pad[0x24]; u8 phase; } *gUnk_0200047C;
extern void sub_0807FC90(void);
void sub_08020D68(void *, u32);
void sub_08020D54(void *, void *);
u32 sub_08065FD0();
u32 sub_08066AB4();
u32 sub_08066DB8();
u32 sub_08065C50();
u32 sub_08065D1C();
void sub_0806FD08(u32, void *, u32);
void sub_08078BB4();
void sub_0807C644();

static inline u8 Has(u16 *p, u32 m)
{
    if (*p & m)
        return 1;
    return 0;
}
static inline u8 IsOne(s32 v)
{
    if (v == 1)
        return 1;
    return 0;
}

u32 sub_0812E7EC(struct Obj *o)
{
    struct Grp *g;
    u32 id;
    struct Cmd *cmd;
    u32 v;
    struct Recv *r;
    struct Recv *r2;
    s32 i;
    u16 buf[3];
    s32 x, y;
    u32 m;

    m = 0x100;
    if (!Has(&o->h2C0, m))
        return 0;
    if (IsOne(o->mode))
        g = (struct Grp *)((u8 *)o + 0x3D4);
    else
        g = 0;
    v = 0;
    id = o->id;
    r = &o->recv;
    r2 = r;
    i = 0;
    for (; i < o->recv.count[gUnk_0200047C->phase]; i++) {
        cmd = r->pending[gUnk_0200047C->phase][i];
        switch (cmd->kind) {
        case 0:
            if (id == 0x12) {
                o->h2FC = cmd->a;
                o->h2FE = cmd->b;
                o->h300 = cmd->c;
                o->b2A8 = 2;
                o->b2A9 = 0;
                o->b2AA = 3;
                o->w2AC = 0;
                o->b2B1 = 1;
                o->b2B2 = 1;
                o->b2B0 = 0;
                o->b2B5 = 0x10;
                o->fn21C = sub_0807FC90;
                o->h2C8 = 0;
                o->flags114 &= ~1;
                o->w104 = 0;
            } else {
                g->ctx->h1C = cmd->a;
                g->ctx->h1E = cmd->b;
                g->ctx->h20 = cmd->c;
                if (cmd->d)
                    o->h2D4 |= 0x200;
                else
                    o->h2D4 &= ~0x200;
                g->ctx->w0C = g->ctx->h1C;
                o->b2A8 = 2;
                o->b2A9 = 0;
                o->b2AA = 3;
                o->w2AC = 0;
                o->b2B1 = 1;
                o->b2B2 = 1;
                o->b2B0 = 0;
                o->b2B5 = 0x10;
                o->fn21C = sub_0807FC90;
                o->h2C8 = 0;
                o->flags114 &= ~1;
                o->w104 = 0;
            }
            break;
        case 1:
            x = o->dir;
            if (x < 0)
                x = -(-x >> 5);
            else
                x >>= 5;
            x = ((x + 3) & 7);
            x = cmd->a - x;
            if (x < 0)
                x = -x;
            if (id == 0x17) {
                if (x == 0)
                    goto recv;
                o->b3CD = ((cmd->kind + 5) & 7) << 5;
                o->b2A8 = 2;
                o->b2A9 = 0;
                o->b2AA = 4;
                o->w2AC = 0;
                o->b2B1 = 1;
                o->b2B2 = 1;
                o->b2B0 = 0;
                o->b2B5 = 0x10;
                o->fn21C = sub_0807FC90;
                o->h2C8 = 0;
                o->flags114 &= ~1;
                o->w104 = 0;
            } else if (x > 1) {
                o->b3CD = cmd->a;
                o->b2A8 = 2;
                o->b2A9 = 0;
                o->b2AA = 4;
                o->w2AC = 0;
                o->b2B1 = 1;
                o->b2B2 = 1;
                o->b2B0 = 0;
                o->b2B5 = 0x10;
                o->fn21C = sub_0807FC90;
                o->h2C8 = 0;
                o->flags114 &= ~1;
                o->w104 = 0;
            } else {
recv:
                sub_08020D68(r2, 1);
            }
            break;
        case 2:
            o->dir = ((cmd->a + 5) & 7) << 5;
            o->b2A8 = 2;
            o->b2A9 = 0;
            o->b2AA = 2;
            o->w2AC = 0;
            o->b2B1 = 1;
            o->b2B2 = 1;
            o->b2B0 = 0;
            o->b2B5 = 0x10;
            o->fn21C = sub_0807FC90;
            o->h2C8 = 0;
            o->flags114 &= ~1;
            o->w104 = 0;
            break;
        case 4:
            o->b2A8 = 2;
            o->b2A9 = 0;
            o->b2AA = 5;
            o->w2AC = 0;
            o->b2B1 = 1;
            o->b2B2 = 1;
            o->b2B0 = 0;
            o->b2B5 = 0x10;
            o->fn21C = sub_0807FC90;
            o->h2C8 = 0;
            o->flags114 &= ~1;
            o->w104 = 0;
            break;
        case 8:
            o->dir = ((cmd->a + 5) & 7) << 5;
            o->b2A8 = 2;
            o->b2A9 = 0;
            o->b2AA = 6;
            o->w2AC = 0;
            o->b2B1 = 1;
            o->b2B2 = 1;
            o->b2B0 = 0;
            o->b2B5 = 0x10;
            o->fn21C = sub_0807FC90;
            o->h2C8 = 0;
            o->flags114 &= ~1;
            o->w104 = 0;
            break;
        case 9:
            o->dir = ((cmd->a + 5) & 7) << 5;
            o->b2A8 = 2;
            o->b2A9 = 0;
            o->b2AA = 7;
            o->w2AC = 0;
            o->b2B1 = 1;
            o->b2B2 = 1;
            o->b2B0 = 0;
            o->b2B5 = 0x10;
            o->fn21C = sub_0807FC90;
            o->h2C8 = 0;
            o->flags114 &= ~1;
            o->w104 = 0;
            break;
        case 10:
            o->dir = ((cmd->a + 5) & 7) << 5;
            o->b2A8 = 2;
            o->b2A9 = 0;
            o->b2AA = 8;
            o->w2AC = 0;
            o->b2B1 = 1;
            o->b2B2 = 1;
            o->b2B0 = 0;
            o->b2B5 = 0x10;
            o->fn21C = sub_0807FC90;
            o->h2C8 = 0;
            o->flags114 &= ~1;
            o->w104 = 0;
            break;
        case 11:
            o->b2A8 = 2;
            o->b2A9 = 0;
            o->b2AA = 9;
            o->w2AC = 0;
            o->b2B1 = 1;
            o->b2B2 = 1;
            o->b2B0 = 0;
            o->b2B5 = 0x10;
            o->fn21C = sub_0807FC90;
            o->h2C8 = 0;
            o->flags114 &= ~1;
            o->w104 = 0;
            break;
        case 5:
            buf[0] = 6;
            goto send;
        case 6:
            buf[0] = 6;
            buf[1] = cmd->a;
            buf[2] = cmd->b;
            goto send;
        case 7:
            if (id == 0x17 || id == 0x12)
                buf[0] = 7;
            else
                buf[0] = 8;
            buf[1] = cmd->a;
send:
            sub_0806FD08(id, buf, o->handle);
            break;
        case 3:
            switch (cmd->a) {
            case 1:
            case 2:
                sub_08078BB4(o, v, 0, 0, id);
                break;
            case 0:
                sub_08078BB4(o, v, 0, 1, id);
                break;
            case 3:
                sub_08078BB4(o, v, 0, 2, id);
                break;
            case 4:
                o->s5AA = cmd->b;
                o->s5A8 = cmd->c;
                v = 0;
                sub_0807C644(id, 1, &x, &y);
                v = sub_08065FD0(0, &o->s5C, o->handle, x, y, id, &o->s5AA, o->s5A8, v, o, &o->handle);
                break;
            case 5:
                o->s5AA = cmd->b;
                o->s5A8 = cmd->c;
                sub_0807C644(id, 3, &x, &y);
                v = sub_08066AB4(7, &o->s5C, o->handle, y, 0, &o->s5AA, o->s5A8, cmd->d, &o->handle, o);
                break;
            case 6:
                sub_0807C644(id, 1, &x, &y);
                v = sub_08066DB8(v, &o->handle, 5, 0, 0, 0, y, x);
                break;
            case 7:
                sub_0807C644(id, 1, &x, &y);
                v = sub_08066DB8(v, &o->handle, 4, 0, 0, 0, y, x);
                break;
            case 8:
                g->w0A0 = sub_08065C50(g->w0A0, 0, o->handle);
                g->w0A0 = sub_08065C50(g->w0A0, 1, o->handle);
                g->w0A0 = sub_08065C50(g->w0A0, 2, o->handle);
                g->w0A0 = sub_08065C50(g->w0A0, 4, o->handle);
                g->w0A0 = sub_08065C50(g->w0A0, 5, o->handle);
                g->w0A0 = sub_08065D1C(g->w0A0, 3, o->handle);
                break;
            }
            sub_08020D68(r2, 1);
            break;
        }
        sub_08020D54(r, cmd);
    }
    return 1;
}
