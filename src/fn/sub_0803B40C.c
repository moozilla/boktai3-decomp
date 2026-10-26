// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct Blob {
    u32 words[8];
};
struct Sprite {
    u32 flags;
    u8 pad4[3], visible;
    u8 pad8[8];
    u16 tile;
    u8 pad12[0x1a];
};
struct Animation {
    const u16 *frames;
    u8 flags, length, duration, period;
    u16 frame, base, speed, timer;
};
struct Drawing {
    u8 bytes[0x60];
};
struct ResultMenu {
    u8 task[0x18];
    u32 timer;
    u8 state, entered;
    u8 pad1e[0x12];
    struct Blob primary, secondary;
    struct Sprite sprite;
    u8 glyph[0x1c];
    struct Drawing draws[22];
    void *onClose, *resource;
    int windows[4];
    u8 left[16], right[16];
};
int Script_SeekToKeyword(int);
void *Script_GetTextSourcePc(void);
void *ScriptProc_Start(int, int);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void AnimData_Load(void *, int);
void AnimData_SelectPaletteBank(void *, int);
void Sprite_Init(void *, void *, int);
void Anim_RestartSequence(void *, void *, int, int, int);
void *WirelessExchange_GetPeerEntry(int);
void Memory_Copy(void *, void *, int);
int WirelessResultPanel_Init(struct ResultMenu *arg)
{
    struct ResultMenu *p = arg;
    struct Blob *res;
    struct Blob *first;
    int pack;
    struct Blob *primary;
    struct Blob *secondary;
    int i;
    void *resource;
    void *glyph;
    struct Saved {
        struct Animation animation;
        u8 *visible;
    } saved;
    struct Animation *a;
    const u16 *frame;
    struct Sprite *sprite;
    struct Sprite *savedSprite;
    int flags;
    gResultMenu = p;
    if (Script_SeekToKeyword(115)) {
        void *value = Script_GetTextSourcePc();
        void **slot = &p->resource;
        *slot = value;
        if (value)
            goto initialize;
    }
failed:
    return -1;
initialize:
    pack = 0xCB05;
    res = ScriptProc_Start(pack, 0x530D);
    if (!res)
        goto failed;
    asm("" : "+r"(res));
    p->primary = *res;
    first = &p->primary;
    ResourceHeader_Relocate(first, res);
    res = ScriptProc_Start(pack, 0x7C8B);
    primary = first;
    if (!res)
        goto failed;
    asm("" : "+r"(res));
    p->secondary = *res;
    secondary = &p->secondary;
    ResourceHeader_Relocate(secondary, res);
    {
        struct Drawing *draw = p->draws;
        u32 drawFlags = 0x2011;
        Widget_InitLinked(draw, secondary, 30, drawFlags, 0, 0, 60, 0);
        draw = p->draws + 1;
        Widget_InitLinked(draw, secondary, 2, drawFlags, 0, 0, 60, 0);
        draw = p->draws + 2;
        glyph = p->glyph;
        savedSprite = &p->sprite;
        {
            register u8 *visible asm("r1") = &p->sprite.visible;
            saved.visible = visible;
        }
        {
            register int zero asm("r5") = 0;
            int count = 19;
            do {
                Widget_InitLinked(draw, primary, 51, 0x2011, zero, zero, 60, (void *)zero);
                count--;
                draw++;
            } while (count >= 0);
        }
    }
    resource = ScriptProc_Start(0x922E, 0x4BBF);
    AnimData_Load(glyph, 0x593A);
    AnimData_SelectPaletteBank(glyph, 503);
    Sprite_Init(savedSprite, glyph, 0x2011);
    Anim_RestartSequence(&saved.animation, resource, 0, 0, 0);
    sprite = savedSprite;
    a = &saved.animation;
    {
        int offset = a->frame << 1;
        const u16 *table;
        table = saved.animation.frames;
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
            register struct ResultMenu *write asm("r3") = p;
            int value = write->sprite.flags;
            value |= 4;
            write->sprite.flags = value;
        } else {
            register struct ResultMenu *write asm("r4") = p;
            int value = write->sprite.flags;
            int mask = -5;
            value &= mask;
            write->sprite.flags = value;
        }
    }
    {
        int observed = a->flags;
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
        u32 initialPeriod;
        register int savedPeriod asm("r6");
        register int bits asm("r2");
        a->timer = elapsed;
        {
            register u32 constant asm("r6") = 65535;
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
            asm("" : "+r"(next));
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
        int zero = 0;
        {
            u8 *visible = saved.visible;
            *visible = zero;
        }
        Memory_Copy(p->left, WirelessExchange_GetPeerEntry(0), 16);
        Memory_Copy(p->right, WirelessExchange_GetPeerEntry(1), 16);
        {
            struct ResultMenu *write = p;
            int one;
            write->state = zero;
            one = 1;
            write->entered = one;
            write->timer = zero;
        }
        {
            void **callback = &p->onClose;
            *callback = (void *)zero;
        }
    }
    return 0;
}
