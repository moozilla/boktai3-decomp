#include "global.h"
// MATCHING_CONCESSION: register-pinning
extern u8 *gUnk_030042E4;
extern const u8 gUnk_0824F174[];
extern const u8 gUnk_0824F17C[];
void *ScriptProc_Start(u32, u32);
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void Collider_Init(void *, u32, u32, u32, u32, const void *, const void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
s32 Collider_Refresh(void *);
void Armor_OnPartStateSound(void);
void Armor_OnPartDamageCollision(void);
struct ArmorInitA {
    u16 h0, h2, h4, h6, h8, ha, hc;
    u8 padE[0xc];
    u16 h1a;
};
struct ArmorInitB {
    u8 b0, b1, b2, pad3[3], b6, b7;
    u8 pad8[6];
    u16 h0e, h10;
    u8 b12, b13;
    u8 pad14[0x30];
    void *owner;
};
struct ArmorInitPart {
    u32 flags;
    u8 pad4[0xc];
    u16 value;
    u8 pad12[0x1a];
    u8 anim[6];
    u16 palette;
    u8 pad34[4];
    void *animPtr;
    u8 pad3c[0xc];
    u8 collider48[6];
    u16 flags4E;
    u8 pad50[0x4c];
    u8 collider9C[6];
    u16 flagsA2;
    u8 padA4[0x4c];
    struct ArmorInitA a;
    u8 pad10C[0xc];
    struct ArmorInitB b;
    u8 pad160[0x6c];
    u32 word1CC;
    u8 pad1D0[4];
    u16 h1D4, h1D6;
    u8 pad1D8[0xc];
};
struct ArmorInit {
    struct ArmorInitPart parts[3];
    u16 x, y, z;
    u8 pad5B2[2];
    u8 *palettePtr;
    u16 value5B8;
    u8 pad5BA[2];
    u16 palette;
    u16 h5BE;
    u8 b5C0, b5C1, b5C2, b5C3, b5C4, b5C5;
};
void Armor_InitParts(struct ArmorInit *p, u32 count, u32 mode)
{
    s32 zero = 0;
    s32 i;
    u8 *animation;
    u32 paletteIndex;
    u32 partValue;
    void *script;
    p->b5C1 = zero;
    p->h5BE = 0x192;
    switch (mode) {
        case 0:

        default: {
            u8 *paletteData;
            partValue = 0;
            p->palette = 0x2BD;
            paletteIndex = p->palette;
            paletteData = gUnk_030042E4 + (paletteIndex << 5);
            p->palettePtr = paletteData;
        } break;

        case 11: {
            u8 *paletteData;
            partValue = 1;
            p->palette = 0x2BD;
            paletteIndex = p->palette;
            paletteData = gUnk_030042E4 + (paletteIndex << 5);
            p->palettePtr = paletteData;
        } break;

        case 3:
            partValue = 2;
            script = ScriptProc_Start(0x92B3, 0xCB8F);
            p->palettePtr = ((u8 *)script) + 0xF4;
            p->palette = 0xF2BD;
            break;

        case 5:
            partValue = 2;
            script = ScriptProc_Start(0x92B3, 0xCB8F);
            p->palettePtr = ((u8 *)script) + 0x154;
            p->palette = 0xF2BD;
            break;

        case 7:

        case 13:
            partValue = 2;
            {
                u8 *paletteData;
                p->palette = 0x216;
                paletteIndex = p->palette;
                paletteData = gUnk_030042E4 + (paletteIndex << 5);
                p->palettePtr = paletteData;
            }
            break;

        case 9:

        case 15:
            partValue = 2;
            script = ScriptProc_Start(0x92B3, 0xCB7F);
            p->palettePtr = ((u8 *)script) + 0xF4;
            p->palette = 0xF2BD;
            break;
    }

    p->b5C3 = count;
    p->b5C4 = 0;
    p->value5B8 = 200;
    p->b5C0 = (p->z = (p->y = (p->x = (p->b5C5 = (p->b5C2 = 0)))));
    for (i = 0; i <= 2; i++) {
        struct ArmorInitPart *q = &p->parts[i];
        animation = q->anim;
        q->a.h0 = 0;
        q->a.h2 = 0;
        q->a.h4 = 0;
        q->a.h8 = 0;
        q->a.ha = 1;
        q->word1CC =
            (q->b.h10 =
                 (q->b.h0e =
                      (q->b.b13 =
                           (q->b.b12 = (q->b.b2 = (q->a.h1a = (q->b.b1 = (q->a.hc = 0))))))));
        AnimData_Load(animation, 0x5292);
        Sprite_Init(q, animation, 0);
        q->flags = 3;
        q->palette = p->palette;
        q->animPtr = p->palettePtr;
        q->value = partValue;
        q->b.b0 = i;
        q->b.b6 = 0;
        q->b.b7 = 0;
        {
            u32 flags = q->b.b1;
            u32 active = 1;
            asm("" : "+r"(active));
            q->b.b1 = active | flags;
        }
        q->b.owner = p;
        q->h1D4 = 0x14;
        q->h1D6 = 0;
        Collider_Init(q->collider48, 0, 0x2001, 0, 0x10, gUnk_0824F174, gUnk_0824F17C);
        Collider_SetCallback(q->collider48, Armor_OnPartStateSound, q);
        Collider_Refresh(q->collider48);
        Collider_SetAttackProperties(q->collider48, 0x14, 10, 0x2000, 0, 30);
        q->flags4E |= 4;
        Collider_Init(q->collider9C, 0, 0x5009, 0, 0x10, gUnk_0824F174, gUnk_0824F17C);
        Collider_SetBodyProperties(q->collider9C, 0x14, 0, 0);
        Collider_SetCallback(q->collider9C, Armor_OnPartDamageCollision, q);
        Collider_Refresh(q->collider9C);
        q->flagsA2 |= 4;
    }

    p->b5C0 = 3;
}
