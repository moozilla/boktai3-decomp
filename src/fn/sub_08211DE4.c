#include "global.h"

extern u8 *gUnk_030042E4;
void VariantSprite_SelectAnimation(void *, u32);
s32 ForgingBook_IsWeaponDiscovered(u32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void PlayerAttackEffect_SetEmissionState(u32);
struct ForgingBookPreviewFrame {
    u16 animationId;
};
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
            s32 discovered = ForgingBook_IsWeaponDiscovered(0x30);
            if (discovered) {
                ((struct ForgingBookPreviewFrame *)(p + 0xaea))->animationId = 0x1b6;
            } else {
                u8 *frameOffset = p + 0xae8;
                u16 *baseFrame;
                void *animation;
                void *keyframes;
                *frameOffset = 3;
                *((u8 *)(p + 0xae9)) = 1;
                baseFrame = (u16 *)(p + 0xaea);
                *baseFrame = discovered;
                animation = p + 0xa88;
                keyframes = p + 0xa68;
                Anim_LoadKeyframe(animation, keyframes, *frameOffset, 2);
                Anim_SetKeyframes(animation, keyframes, (u16)((*baseFrame) + (*frameOffset)), 2, 4);
                *((s32 *)(p + 0x20)) = discovered;
                return 0;
            }
            break;
        }

        case 6:
            ((struct ForgingBookPreviewFrame *)(p + 0xaea))->animationId = 0x1bc;
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
        s16 *category = (s16 *)(p + 0xb60);
        if ((*category) <= 5) {
            u8 *frameOffset = p + 0xae8;
            void *animation;
            void *keyframes;
            u16 *baseFrame;
            *frameOffset = 3;
            *((u8 *)(p + 0xae9)) = 1;
            animation = p + 0xa88;
            keyframes = p + 0xa68;
            baseFrame = (u16 *)(p + 0xaea);
            Anim_LoadKeyframe(animation, keyframes, (u16)((*baseFrame) + (*frameOffset)), 2);
            Anim_SetKeyframes(animation, keyframes, (u16)((*baseFrame) + (*frameOffset)), 2, 4);
            if ((*category) <= 4) {
                PlayerAttackEffect_SetEmissionState(2);
                PlayerAttackEffect_SetEmissionState(1);
                if ((*category) == 0)
                    PlayerAttackEffect_SetEmissionState(3);
                if ((*category) == 4)
                    PlayerAttackEffect_SetEmissionState(2);
            } else {
                struct FbPreview *src = (struct FbPreview *)(gUnk_030042E4 + 0x6a0);
                u16 value = src->a;
                *((u16 *)(p + 0xaf6)) = value;
                value = src->b;
                *((u16 *)(p + 0xaf8)) = value;
                value = src->c;
                *((u16 *)(p + 0xb06)) = value;
            }
        } else {
            *((u8 *)(p + 0xae8)) = 0;
            if ((*category) == 8)
                *((u8 *)(p + 0xae9)) = 0;
            else
                *((u8 *)(p + 0xae9)) = 1;
            {
                void *animation = p + 0xa88;
                void *keyframes = p + 0xa68;
                u16 *baseFrame = (u16 *)(p + 0xaea);
                u8 *emissionMode;
                u16 frame = *baseFrame;
                emissionMode = p + 0xbe5;
                Anim_LoadKeyframe(animation, keyframes, frame, *emissionMode);
                Anim_SetKeyframes(animation, keyframes, *baseFrame, *emissionMode, 4);
            }
        }
    }
    *((s32 *)(p + 0x20)) = 0;
    return 0;
}
