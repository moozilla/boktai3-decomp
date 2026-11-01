#include "global.h"
#include "sword_orbit.h"
#include "engine_globals.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
struct StretchBody {
    u8 pad0[0x60];
    u8 *body;
    u8 pad64[0xB20];
    u16 x, y, z;
    u8 padB8A[0x18A];
    u16 value;
    u8 padD16[0x16];
    u8 count;
    u8 base;
};
struct StretchPart {
    u32 flags;
    u8 pad4[2];
    u8 rotation;
    u8 pad7;
    s8 byte8;
    s8 byte9;
    u8 padA[0x12];
    u16 x, y, z;
    u8 pad22[0x2c];
    u16 flags4E;
    u8 pad50[2];
    u16 param52;
    u8 pad54[0x4e];
    u16 flagsA2;
    u8 padA4[2];
    u16 paramA6;
    u8 padA8[0x62];
    u16 id10A;
    u8 pad10C[0xc];
    u8 index118;
    u8 flags119;
    u8 pad11A[0x42];
    u16 *target;
    struct StretchBody *parent;
    u8 pad164[8];
    void (*callback)(struct StretchPart *);
    u8 pad170[0x19];
    u8 angle;
    s8 spin;
};
static inline u8 reset(u8 *p)
{
    s32 one = 1;
    struct F {
        u8 flags;
    } *flag = (struct F *)(p + 0x119);
    u8 old = flag->flags;
    if (old & one) {
        s32 mask = ~1;
        s32 value = mask;
        s32 zero;
        value &= old;
        zero = 0;
        flag->flags = value;
        p[0x12e] = zero;
        return 1;
    }
    return 0;
}
void Ivy_BeginChildPulse(struct StretchPart *);
// confidence=low: positions components around their parent; the resting-state role is tentative.
void Callback_InvokeActor(void *, void *);
void Sword_BeginRestOrbit(struct StretchPart *p)
{
    if (reset((u8 *)p)) {
        struct StretchBody *parent = p->parent;
        s32 one;
        s32 choice;
        p->id10A = parent->value;
        p->angle = parent->base + Div(256, parent->count) * p->index118;
        p->x = parent->x + Div(gUnk_086149C4[(p->angle + 64) & 255] * 200, 4096);
        p->y = *(u16 *)(parent->body + 0x32) + 220;
        p->z = parent->z + Div(gUnk_086149C4[p->angle] * 200, 4096);
        p->flags &= ~1;
        p->rotation = 128;
        p->byte8 = 127;
        one = 1;
        p->byte9 = one;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 offset = gRandomIndex * 2;
            const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
            choice = *(const u16 *)(offset + (u32)table);
        }
        choice &= one;
        p->spin = Sword_OrbitDirection(choice);
        p->target = &parent->x;
        p->flagsA2 &= ~4;
        p->flags4E &= ~4;
        p->paramA6 = 16;
        p->param52 = 16;
        {
            u32 off = 0x16C;
            u32 addr;
            addr = (u32)p + off;
            *(void **)addr = Ivy_BeginChildPulse;
        }
    }
    {
        u32 off = 0x16c;
        u8 *address;
        void *callback;
        address = (u8 *)p + off;
        callback = *(void **)address;
        if (callback)
            Callback_InvokeActor(p, callback);
    }
}
