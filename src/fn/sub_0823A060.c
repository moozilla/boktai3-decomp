#include "global.h"
struct LinkSide {
    u8 pad0[0x18];
    s16 side;
};
struct LinkAura {
    u8 pad0[0x18];
    s32 owner;
    u32 f1c;
    u32 f20;
    u8 pad24[0x94 - 0x24];
    u32 flags;
    u8 padLine98[0xc0 - 0x98];
    u8 fadeA, fadeB;
    u8 padC2[0x14c - 0xc2];
    u32 selFlags;
    u8 pad150[0x4e1 - 0x150];
    u8 chimePending;
    u8 pad4E2[0x534 - 0x4e2];
    u16 state;
    u16 pad536;
    u16 active;
    u8 pad53A[0xaf2 - 0x53a];
    u16 fade;
};
extern struct LinkSide *gLinkSession;
extern struct LinkAura *gUnk_02000580[];
void Player_PlayLocalSong(s32, s32);
static inline s32 LinkAura_LocalSide(void)
{
    struct LinkSide *a = gLinkSession;
    if (a == 0)
        return -1;
    return a->side;
}
static inline s32 LinkAura_IsInStateFour(struct LinkAura *other)
{
    if (other->active != 0 && other->state == 4)
        return 1;
    return 0;
}
static inline void LinkAura_SetFlags(u32 *f, u32 mask)
{
    *f |= mask;
}
static inline void LinkAura_FadeOut(struct LinkAura *p, u32 zero)
{
    if (p->fade != 0) {
        u8 *chime = &p->chimePending;
        if (*chime != 0) {
            Player_PlayLocalSong(p->owner, 0x336);
            *chime = zero;
        }
        p->fade -= 1;
        if (p->fade == 0) {
            p->flags &= ~2;
            p->fadeA = 0x40;
            p->fadeB = 0x40;
        } else {
            s32 v;
            p->flags |= 2;
            v = 0x40 - (p->fade >> 1);
            p->fadeA = v;
            p->fadeB = v;
        }
    }
}
void sub_0823A060(struct LinkAura *p)
{
    u16 active = p->active;
    if (active != 0) {
        if (p->state == 5) {
            s32 v;
            if (p->fade <= 0x3f) {
                if (p->fade == 0) {
                    Player_PlayLocalSong(p->owner, 0x1ee);
                    p->chimePending = 1;
                }
                p->fade += 1;
            }
            p->flags |= 2;
            v = 0x40 - (p->fade >> 1);
            p->fadeA = v;
            p->fadeB = v;
            LinkAura_SetFlags(&p->f20, 0x50000);
        } else {
            LinkAura_FadeOut(p, 0);
        }
        if (p->state == 6) {
            s32 side = LinkAura_LocalSide();
            struct LinkAura *other;
            if (p->owner == side) {
                p->selFlags |= 0x200;
                p->flags |= 0x200;
                LinkAura_SetFlags(&p->f20, 0x20000);
            } else if ((other = gUnk_02000580[side]) != 0 && LinkAura_IsInStateFour(other)) {
                p->selFlags |= 0x200;
                p->flags |= 0x200;
                LinkAura_SetFlags(&p->f20, 0x20000);
            } else {
                p->selFlags |= 1;
                p->flags |= 1;
                LinkAura_SetFlags(&p->f20, 0x20000);
            }
        } else {
            p->selFlags &= ~0x200;
            p->flags &= ~0x200;
        }
        if (p->f1c & 1)
            p->active--;
    } else {
        LinkAura_FadeOut(p, active);
        if (p->flags & 0x200)
            p->flags &= ~0x200;
        if (p->selFlags & 0x200)
            p->selFlags &= ~0x200;
    }
}
