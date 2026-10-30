// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
void Sound_PlaySong(u32);

static inline u8 HasMode(u8 *p)
{
    u32 mask = 1;
    if (*(u16 *)(p + 0x65C) & mask)
        return 1;
    return 0;
}

u8 Mimic_StartHitReaction(struct SceneActorRegistryResetView *actor)
{
    u8 *ctx = actor->context;
    u32 off;
    if (actor->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    off = 0x2DC;
    asm("" : "+r"(off));
    if (*(u32 *)((u8 *)actor + off) & 0xC0000) {
        u32 o4 = 0x230;
        register u32 kind asm("r1");
        register u32 control asm("r2");
        u32 one;
        register u32 zero asm("r3");
        u32 z4;
        register u8 *addr asm("r0");
        control = *(u32 *)((u8 *)actor + o4);
        kind = 0x12;
        off -= 0x2A;
        addr = (u8 *)actor + off;
        zero = 0;
        one = 1;
        *addr = one;
        o4 += 0x80;
        addr = (u8 *)actor + o4;
        *addr = zero;
        o4 += 5;
        addr = (u8 *)actor + o4;
        *addr = kind;
        {
            u32 ot = 0x21C;
            u32 *w;
            w = (u32 *)((u8 *)actor + ot);
            *w = control;
        }
        {
            u32 o = 0x2C8;
            u16 *h;
            h = (u16 *)((u8 *)actor + o);
            z4 = 0;
            *h = zero;
        }
        {
            u32 o = 0x114;
            u32 *fl;
            u32 m;
            fl = (u32 *)((u8 *)actor + o);
            m = ~1;
            *fl = m & *fl;
        }
        {
            register u32 o asm("r1") = 0x104;
            register u32 *w asm("r0");
            asm("" : "+r"(o));
            w = (u32 *)((u8 *)actor + o);
            *w = zero;
        }
        {
            u32 three = 3;
            u32 o = 0x2A8;
            register u32 o1 asm("r1");
            addr = (u8 *)actor + o;
            *addr = three;
            o1 = 0x2A9;
            addr = (u8 *)actor + o1;
            *addr = z4;
            o += 2;
            addr = (u8 *)actor + o;
            *addr = z4;
            {
                register u32 oc asm("r4") = 0x2AC;
                register u32 *w asm("r0");
                asm("" : "+r"(oc));
                w = (u32 *)((u8 *)actor + oc);
                *w = zero;
            }
            o1 += 8;
            addr = (u8 *)actor + o1;
            *addr = one;
        }
        return 1;
    }
    {
        u32 mask;
        u16 *flags;
        u32 value;
        off = 0xB5 << 2;
        asm("" : "+r"(off));
        flags = (u16 *)((u8 *)actor + off);
        mask = ~0x20;
        value = *flags;
        mask &= value;
        *flags = mask;
    }
    if (actor->tracking.target) {
        if (HasMode(actor->context)) {
            {
                register u32 mask asm("r0") = 0x80;
                register u32 o asm("r3") = 0x2D6;
                register u16 *flags asm("r2");
                asm("" : "+r"(o));
                flags = (u16 *)((u8 *)actor + o);
                *flags = mask | *flags;
            }
            {
                register u32 o asm("r4") = 0x3D0;
                register u8 *c asm("r0");
                u8 *mode;
                u32 k;
                u32 mask;
                u16 *mp;
                asm("" : "+r"(o));
                c = (u8 *)actor + o;
                mode = *(u8 **)c;
                k = 0x65C;
                mp = (u16 *)(mode + k);
                mask = ~1;
                {
                    u32 value;
                    value = *mp;
                    mask &= value;
                    *mp = mask;
                }
            }
        } else if (actor->health < ({
                       register u32 ho asm("r3") = 0x660;
                       register s32 *hp asm("r0");
                       asm("" : "+r"(ho));
                       hp = (s32 *)(ctx + ho);
                       *hp;
                   })) {
            {
                register u32 mask asm("r0") = 0x80;
                register u32 o asm("r4") = 0x2D6;
                register u16 *flags asm("r2");
                asm("" : "+r"(o));
                flags = (u16 *)((u8 *)actor + o);
                *flags = mask | *flags;
            }
            {
                u32 o = 0x3D0;
                u8 *c;
                u8 *mode;
                u32 k;
                u32 mask;
                u16 *mp;
                c = (u8 *)actor + o;
                mode = *(u8 **)c;
                k = 0x65C;
                mp = (u16 *)(mode + k);
                mask = ~1;
                {
                    u32 value;
                    value = *mp;
                    mask &= value;
                    *mp = mask;
                }
            }
        }
    }
    return 0;
}
