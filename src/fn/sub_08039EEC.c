// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Pair {
    u32 x, y;
};
struct Vec {
    s16 x, y, z;
};
struct Sprite {
    u32 flags;
    u8 pad4[12];
    u16 value;
    u8 pad12[10];
    u32 x, y;
    u8 pad24[8];
};
struct Drawing {
    u8 pad0[8];
    u32 flags;
    u8 padc[0x14];
    u32 x, y;
    u8 pad28[0x38];
};
struct Item {
    u8 state, entered, unused, count, type, pad5;
    s16 velocity;
    u16 timer;
    u8 pada[2];
    struct Vec start;
    struct Vec end;
    u8 pad1c[8];
    struct Sprite sprite;
    struct Drawing drawing;
};
struct Owner {
    u8 pad0[0x18];
    u8 current[4], pending[4];
    u8 pad20[0x1c];
    u8 resource[32];
    struct Item items[16];
};
extern const u16 gUnk_0824DE7C[], gUnk_0824DE8C[];
void Anim_LoadKeyframe(void *, void *, int, int);
void sub_08039968(void *, int, int);
void sub_082216F8(void *, void *, void *, int, int);
void sub_08039998(void *, int, int);
void sub_08039EEC(struct Owner *owner, struct Item *p, int indexArg)
{
    int index = indexArg;
    struct Sprite *sprite;
    struct Drawing *drawing = &p->drawing;
    sprite = &p->sprite;
    if (p->entered) {
        int choice;
        int y;
        register void *draw asm("r3");
        void *resource;
        p->entered = 0;
        {
            int value = p->count;
            struct Sprite *dest;
            dest = sprite;
            dest->value = value;
        }
        p->sprite.flags &= ~1;
        draw = drawing;
        resource = owner->resource;
        if (index > 7)
            choice = (index & 3) + 4;
        else
            choice = index & 3;
        Anim_LoadKeyframe(draw, resource, gUnk_0824DE7C[choice], 0);
        *(u32 *)((u8 *)p + 0x58) &= ~1;
        {
            struct Vec *start = &p->start;
            const u16 *entry;
            int x;
            int zero;
            struct Vec *end;
            int y;
            {
                const u16 *table = gUnk_0824DE8C;
                int offset;
                offset = (index & 3) * 2;
                entry = (const u16 *)(offset + (u32)table);
            }
            {
                register int initial asm("r1") = 272;
                asm("" : "+r"(initial));
                x = initial;
            }
            asm("" : "+r"(x), "+r"(entry));
            {
                register int value asm("r1") = *entry;
                x += value;
            }
            zero = 0;
            p->start.x = x;
            start->y = 32;
            start->z = zero;
            end = &p->end;
            y = 128;
            if (index > 7)
                y = 144;
            p->end.x = *entry + 80;
            end->y = y;
            end->z = zero;
        }
        sub_08039968(p, 32, 15);
    }
    sub_082216F8(&drawing->x, &p->start, &p->end, p->timer, 5);
    sub_08039998(&drawing->x, p->timer, p->velocity);
    {
        u32 x = drawing->x;
        u32 y = drawing->y;
        register struct Sprite *dest asm("r2");
        dest = sprite;
        dest->x = x;
        dest->y = y;
    }
    if (p->timer > 31) {
        int slot = index & 3;
        u8 *current;
        register u8 *pending asm("r12");
        struct Item *target;
        int count;
        if (p->unused) {
            u8 *array = owner->pending;
            u8 *dest = array + slot;
            register int total asm("r1") = *dest;
            total += p->count;
            pending = array;
            if (total > 255)
                total = 255;
            *dest = total;
            current = owner->current;
        } else {
            u8 *dest;
            int total;
            current = owner->current;
            dest = current + slot;
            total = *dest;
            total += p->count;
            if (total > 255)
                total = 255;
            *dest = total;
            {
                int offset = 0x1c;
                u8 *array;
                asm("" : "+r"(offset));
                array = (u8 *)(offset + (u32)owner);
                pending = array;
            }
        }
        {
            int offset = (index & ~4) * 0xb0 + 0x5c;
            int first;
            int second;
            register int one asm("r0");
            int zero;
            target = (struct Item *)((u8 *)owner + offset);
            {
                u8 *address = current + slot;
                first = *address;
                current = pending;
                asm("" : "+r"(current));
                address = current + slot;
                second = *address;
            }
            one = 1;
            zero = 0;
            asm("" : "+r"(one), "+r"(zero));
            target->state = one;
            target->entered = one;
            target->timer = zero;
            {
                register int value asm("r0") = first + second;
                int clear;
                if (value > 10)
                    value = 10;
                clear = 0;
                asm("" : "+r"(clear));
                target->count = value;
                value = 0;
                target->unused = value;
                target->type = 8;
                p->count = clear;
                p->state = clear;
                p->entered = 1;
                p->timer = clear;
            }
        }
    }
    p->timer++;
}
