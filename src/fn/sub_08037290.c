// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Tentative layouts for the wireless peer panel and its initial animation.
struct Sprite {
    u32 flags;
    u8 pad4[3], visible;
    u8 pad8[8];
    u16 tile;
    u8 pad12[10];
    u16 x, y;
    u8 pad20[12];
};

struct Animation {
    const u16 *frames;
    u8 flags, length, duration, period;
    u16 frame, base, speed, timer;
};

struct PeerPanel {
    u8 active, pad1[3];
    u32 flags;
    struct Sprite sprite;
    u8 glyph[0x1C];
    u8 drawings[5][0x60];
    u8 name[16];
    u8 pad240[4];
    u8 changed;
};

struct PeerStyle {
    int resource;
    u8 tile, animation, priority, x, y, pad9[3];
};

struct Owner {
    u8 pad[0x5F4];
    u8 descriptor[1];
};

extern const struct PeerStyle gUnk_0824DE64[];
void *sub_0821A520(int, int);
void sub_082151E4(void *, int);
void sub_08215284(void *, int);
void sub_082144A4(void *, void *, int);
void sub_08220D20(void *, void *, int, int, int);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);

int sub_08037290(struct Owner *owner, struct PeerPanel *panel, int kind)
{
    register struct PeerPanel *p asm("r9") = panel;
    register int type asm("r10") = kind;
    register void *resource asm("r8");
    register void *glyph asm("r5");
    const struct PeerStyle *style;
    struct Sprite *sprite;
    register struct Sprite *initialSprite asm("r6");
    struct Animation animation;
    register const u16 *frame asm("r6");
    register struct Animation *a asm("r5");
    int flags;

    resource = sub_0821A520(0x922E, 0x4BBF);
    glyph = p->glyph;
    {
        register const struct PeerStyle *table asm("r0") = gUnk_0824DE64;
        asm("" : "+r"(table));
        {
            register int index asm("r1") = type;
            asm("" : "+r"(index));
            {
                register u32 offset asm("r4") = index << 1;
                asm("" : "+r"(offset));
                offset += type;
                offset <<= 2;
                style = (const struct PeerStyle *)(offset + (u32)table);
            }
        }
    }
    sub_082151E4(glyph, style->resource);
    {
        register int tile asm("r1") = style->tile;
        register int base asm("r2") = 503;
        asm("" : "+r"(base));
        sub_08215284(glyph, tile + base);
    }
    initialSprite = &p->sprite;
    sub_082144A4(initialSprite, glyph, style->priority | 48);
    sub_08220D20(&animation, resource, style->animation, 0, 0);
    sprite = initialSprite;
    a = &animation;
    {
        register int offset asm("r0") = a->frame << 1;
        register const u16 *table asm("r1");
        asm("" : "+r"(offset));
        table = animation.frames;
        frame = (const u16 *)((u32)table + offset);
    }
    sprite->tile = *frame >> 6;

    // Combine the animation and frame orientation flags for the sprite.
    {
        register int observed asm("r0") = a->flags;
        register int one asm("r3");
        register int side asm("r1");
        register int word asm("r2");
        register u32 encoded asm("r0");
        asm("" : "+r"(observed));
        one = 1;
        side = one;
        asm("" : "+r"(side));
        side &= observed;
        word = *frame;
        encoded = word & 48;
        asm("" : "+r"(encoded));
        encoded >>= 4;
        encoded &= one;
        if (side != encoded) {
            register struct PeerPanel *write asm("r3") = p;
            register int value asm("r0") = write->sprite.flags;
            value |= 4;
            write->sprite.flags = value;
        } else {
            register struct PeerPanel *read asm("r1") = p;
            register int value asm("r0");
            register int mask asm("r1");
            register struct PeerPanel *write asm("r2");
            asm("" : "+r"(read));
            value = read->sprite.flags;
            mask = -5;
            value &= mask;
            write = p;
            write->sprite.flags = value;
        }
    }
    {
        register int observed asm("r4") = a->flags;
        register int two asm("r3") = 2;
        register int side asm("r1");
        register int word asm("r2");
        register u32 encoded asm("r0");
        side = two;
        asm("" : "+r"(side));
        side &= observed;
        asm("" : "+r"(side));
        side = (u8)side;
        word = *frame;
        encoded = word & 48;
        asm("" : "+r"(encoded));
        encoded >>= 4;
        encoded &= two;
        if (side != encoded)
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
        flags = observed;
    }
    {
        register u32 elapsed asm("r0") = a->timer + 1;
        register u32 limit asm("r3");
        register u32 initialPeriod asm("r1");
        register int savedPeriod asm("r6");
        register int bits asm("r2");
        a->timer = elapsed;
        {
            register u32 constant asm("r1") = 65535;
            asm("" : "+r"(constant));
            limit = constant;
        }
        initialPeriod = a->period;
        elapsed = (u16)elapsed;
        savedPeriod = initialPeriod;
        if (elapsed < initialPeriod)
            goto animationDone;
        a->timer = 0;
        bits = flags;
        asm("" : "+r"(bits));
        if (bits & 4) {
            register int previous asm("r0");
            previous = a->frame;
            if (previous)
                goto decrementFrame;
            if (bits & 8) {
                a->timer = savedPeriod;
                goto recalculate;
            }
            previous = a->length;
        decrementFrame:
            previous--;
            a->frame = previous;
            goto recalculate;
        } else {
            register u32 next asm("r0") = a->frame + 1;
            register int length asm("r2");
            a->frame = next;
            length = a->length;
            next &= limit;
            if (next < (u32)length)
                goto recalculate;
            next = flags & 8;
            asm("" : "+r"(next));
            next = (u8)next;
            if (next) {
                a->timer = initialPeriod;
                next = length - 1;
            }
            a->frame = next;
        }
    recalculate:
        {
            register int index asm("r0") = a->frame << 1;
            register const u16 *table asm("r1");
            register int word asm("r1");
            register int value asm("r0");
            asm("" : "+r"(index));
            table = a->frames;
            frame = (const u16 *)((u32)table + index);
            word = *frame;
            value = word & 15;
            a->duration = value;
            asm("" : : : "memory");
            value = a->duration * a->speed;
            asm("" : "+r"(value));
            value >>= 6;
            a->period = value;
            value = (u32)value << 24;
            asm("" : "+r"(value));
            if (!value)
                a->period = 1;
        }
    animationDone:
        ;
    }
    {
        register int one asm("r0") = 1;
        register struct PeerPanel *write asm("r2");
        register const struct PeerStyle *table asm("r1");
        register int kind asm("r3");
        register u32 offset asm("r0");
        register const struct PeerStyle *entry asm("r0");
        asm("" : "+r"(one));
        write = p;
        write->sprite.visible = one;
        table = gUnk_0824DE64;
        asm("" : "+r"(table));
        kind = type;
        offset = kind << 1;
        asm("" : "+r"(offset));
        offset += type;
        offset <<= 2;
        entry = (const struct PeerStyle *)(offset + (u32)table);
        write->sprite.x = entry->x;
        write->sprite.y = entry->y;
    }
    {
        register int zero asm("r6") = 0;
        register u8 *drawing asm("r4");
        register int count asm("r5");
        asm("" : "+r"(zero));
        drawing = p->drawings[0];
        asm("" : "+r"(drawing));
        count = 4;
        do {
            sub_0821983C(drawing, owner->descriptor, 0, 17, 1, zero, 60, (void *)zero);
            drawing += 0x60;
        } while (--count >= 0);
    }
    {
        register int zero asm("r0") = 0;
        register struct PeerPanel *write asm("r1") = p;
        asm("" : "+r"(zero));
        write->active = zero;
        {
            register u8 *name asm("r1") = p->name;
            asm("" : "+r"(name));
            *name = zero;
        }
        {
            register u8 *changed asm("r1") = &p->changed;
            asm("" : "+r"(changed));
            *changed = zero;
        }
    }
}
