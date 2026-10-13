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
void *ScriptProc_Start(int, int);
void AnimData_Load(void *, int);
void AnimData_SelectTiles(void *, int);
void Sprite_Init(void *, void *, int);
void sub_08220D20(void *, void *, int, int, int);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);

int sub_08037290(struct Owner *owner, struct PeerPanel *panel, int kind)
{
    register struct PeerPanel *p asm("r9") = panel;
    int type = kind;
    void *resource;
    void *glyph;
    const struct PeerStyle *style;
    struct Sprite *sprite;
    struct Sprite *initialSprite;
    struct Animation animation;
    const u16 *frame;
    struct Animation *a;
    int flags;

    resource = ScriptProc_Start(0x922E, 0x4BBF);
    glyph = p->glyph;
    {
        const struct PeerStyle *table = gUnk_0824DE64;
        {
            register int index asm("r1") = type;
            {
                u32 offset = index << 1;
                offset += type;
                offset <<= 2;
                style = (const struct PeerStyle *)(offset + (u32)table);
            }
        }
    }
    AnimData_Load(glyph, style->resource);
    {
        int tile = style->tile;
        register int base asm("r2") = 503;
        AnimData_SelectTiles(glyph, tile + base);
    }
    initialSprite = &p->sprite;
    Sprite_Init(initialSprite, glyph, style->priority | 48);
    sub_08220D20(&animation, resource, style->animation, 0, 0);
    sprite = initialSprite;
    a = &animation;
    {
        int offset = a->frame << 1;
        const u16 *table;
        table = animation.frames;
        frame = (const u16 *)((u32)table + offset);
    }
    sprite->tile = *frame >> 6;

    // Combine the animation and frame orientation flags for the sprite.
    {
        int observed = a->flags;
        register int one asm("r3");
        register int side asm("r1");
        register int word asm("r2");
        u32 encoded;
        one = 1;
        side = one;
        side &= observed;
        word = *frame;
        encoded = word & 48;
        encoded >>= 4;
        encoded &= one;
        if (side != encoded) {
            register struct PeerPanel *write asm("r3") = p;
            int value = write->sprite.flags;
            value |= 4;
            write->sprite.flags = value;
        } else {
            register struct PeerPanel *read asm("r1") = p;
            int value;
            int mask;
            register struct PeerPanel *write asm("r2");
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
        u32 encoded;
        side = two;
        side &= observed;
        side = (u8)side;
        word = *frame;
        encoded = word & 48;
        encoded >>= 4;
        encoded &= two;
        if (side != encoded)
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
        flags = observed;
    }
    {
        u32 elapsed = a->timer + 1;
        u32 limit;
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
        if (bits & 4) {
            int previous;
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
            u32 next = a->frame + 1;
            int length;
            a->frame = next;
            length = a->length;
            next &= limit;
            if (next < (u32)length)
                goto recalculate;
            next = flags & 8;
            next = (u8)next;
            if (next) {
                a->timer = initialPeriod;
                next = length - 1;
            }
            a->frame = next;
        }
    recalculate: {
        int index = a->frame << 1;
        const u16 *table;
        register int word asm("r1");
        register int value asm("r0");
        table = a->frames;
        frame = (const u16 *)((u32)table + index);
        word = *frame;
        value = word & 15;
        a->duration = value;
        value = a->duration * a->speed;
        asm("" : "+r"(value));
        value >>= 6;
        a->period = value;
        value = (u32)value << 24;
        if (!value)
            a->period = 1;
    }
    animationDone:;
    }
    {
        int one = 1;
        struct PeerPanel *write;
        const struct PeerStyle *table;
        int kind;
        u32 offset;
        const struct PeerStyle *entry;
        write = p;
        write->sprite.visible = one;
        table = gUnk_0824DE64;
        kind = type;
        offset = kind << 1;
        offset += type;
        offset <<= 2;
        entry = (const struct PeerStyle *)(offset + (u32)table);
        write->sprite.x = entry->x;
        write->sprite.y = entry->y;
    }
    {
        int zero = 0;
        u8 *drawing;
        register int count asm("r5");
        drawing = p->drawings[0];
        count = 4;
        do {
            sub_0821983C(drawing, owner->descriptor, 0, 17, 1, zero, 60, (void *)zero);
            drawing += 0x60;
        } while (--count >= 0);
    }
    {
        register int zero asm("r0") = 0;
        struct PeerPanel *write = p;
        asm("" : "+r"(zero));
        write->active = zero;
        {
            u8 *name = p->name;
            *name = zero;
        }
        {
            u8 *changed = &p->changed;
            *changed = zero;
        }
    }
}
