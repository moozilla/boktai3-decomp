#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining: switch tail sharing, address derivation of the case 5 and category<=5 blocks.
extern u8 *gUnk_030042E4;
void VariantSprite_SelectAnimation(void *, u32);
s32 ForgingBook_IsWeaponDiscovered(u32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void PlayerAttackEffect_SetEmissionState(u32);
struct FbPreview {
    u16 pad[12];
    u16 a, b, c;
};
s32 ForgingBook_RefreshCategoryPreview(u8 *p)
{
    *((u8 *)(p + 0xbe5)) = 2;
    CpuSet(gUnk_030042E4 + 0x540, p + 0xaec, 0x10);
    switch (*((s16 *)(p + 0xb60))) {
        case 0:
            *((u16 *)(p + 0xb0c)) = 0;
            VariantSprite_SelectAnimation(*((void **)(p + 0xb58)), 1);
            *((u16 *)(p + 0xaea)) = 0x5e;
            break;

        case 1:
            VariantSprite_SelectAnimation(*((void **)(p + 0xb58)), 3);
            *((u16 *)(p + 0xaea)) = 0x54;
            break;

        case 2:
            VariantSprite_SelectAnimation(*((void **)(p + 0xb58)), 4);
            *((u16 *)(p + 0xaea)) = 0x54;
            break;

        case 3:
            VariantSprite_SelectAnimation(*((void **)(p + 0xb58)), 0);
            *((u16 *)(p + 0xaea)) = 0x54;
            break;

        case 4:
            VariantSprite_SelectAnimation(*((void **)(p + 0xb58)), 2);
            *((u16 *)(p + 0xaea)) = 0x81;
            break;

        case 5: {
            s32 w = ForgingBook_IsWeaponDiscovered(0x30);
            if (w) {
                *((u16 *)(p + 0xaea)) = 0x1b6;
            } else {
                u8 *r4 = p + 0xae8;
                u16 *r5 = (u16 *)(p + 0xaea);
                void *a = p + 0xa88;
                void *b = p + 0xa68;
                *r4 = 3;
                *((u8 *)(p + 0xae9)) = 1;
                *r5 = w;
                Anim_LoadKeyframe(a, b, *r4, 2);
                Anim_SetKeyframes(a, b, (u16)((*r5) + (*r4)), 2, 4);
                *((s32 *)(p + 0x20)) = w;
                return 0;
            }
            break;
        }

        case 6: {
            u16 *d2 = (u16 *)(p + 0xAE4);
            d2[3] = 0x1bc;
        }
            *((u8 *)(p + 0xbe5)) = 1;
            break;

        case 7:
            *((u16 *)(p + 0xaea)) = 0x89;
            *((u8 *)(p + 0xbe5)) = 2;
            break;

        case 8:
            *((u16 *)(p + 0xaea)) = 0x39;
            *((u8 *)(p + 0xbe5)) = 2;
            break;

        case 9:
            *((u16 *)(p + 0xaea)) = 8;
            *((u8 *)(p + 0xbe5)) = 1;
            break;
    }

    {
        s16 *cat = (s16 *)(p + 0xb60);
        if ((*cat) <= 5) {
            u8 *r4 = p + 0xae8;
            void *a = p + 0xa88;
            void *b = p + 0xa68;
            u16 *r5 = (u16 *)(p + 0xaea);
            *r4 = 3;
            *((u8 *)(p + 0xae9)) = 1;
            Anim_LoadKeyframe(a, b, (u16)((*r5) + (*r4)), 2);
            Anim_SetKeyframes(a, b, (u16)((*r5) + (*r4)), 2, 4);
            if ((*cat) <= 4) {
                PlayerAttackEffect_SetEmissionState(2);
                PlayerAttackEffect_SetEmissionState(1);
                if ((*cat) == 0)
                    PlayerAttackEffect_SetEmissionState(3);
                if ((*cat) == 4)
                    PlayerAttackEffect_SetEmissionState(2);
            } else {
                struct FbPreview *src = (struct FbPreview *)(gUnk_030042E4 + 0x6a0);
                *((u16 *)(p + 0xaf6)) = src->a;
                {
                    u16 *d1 = (u16 *)(p + 0xAF6);
                    d1[1] = src->b;
                }
                *((u16 *)(p + 0xb06)) = src->c;
            }
        } else {
            *((u8 *)(p + 0xae8)) = 0;
            if ((*cat) == 8)
                *((u8 *)(p + 0xae9)) = 0;
            else
                *((u8 *)(p + 0xae9)) = 1;
            {
                void *a = p + 0xa88;
                void *b = p + 0xa68;
                u16 *r5 = (u16 *)(p + 0xaea);
                u8 *r4 = p + 0xbe5;
                Anim_LoadKeyframe(a, b, *r5, *r4);
                Anim_SetKeyframes(a, b, *r5, *r4, 4);
            }
        }
    }
    *((s32 *)(p + 0x20)) = 0;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", ForgingBook_RefreshCategoryPreview);
#endif
