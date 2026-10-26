#include "global.h"

struct S080FB4EC_Ctx {
    u8 unk0[0x90A];
    u16 unk90A;
    u8 unk90C[0xECE - 0x90C];
    u8 unkECE;
    u8 unkECF;
    u8 unkED0;
    u8 unkED1;
};
struct S080FB4EC_Drawable {
    u8 unk0[0x32];
    u16 unk32;
    u8 unk34[0x38 - 0x34];
    void *unk38;
};
struct S080FB4EC_Actor {
    void *object;
    struct S080FB4EC_Drawable *drawable;
    u8 unk8[0x1E0 - 8];
    u16 unk1E0;
    u8 unk1E2[0x1E6 - 0x1E2];
    u16 unk1E6;
    u8 unk1E8[0x3D0 - 0x1E8];
    struct S080FB4EC_Ctx *context;
};
void AnimData_SelectPaletteBank(void *, u32 animation);

// scope shape 2 (differences once low registers are set aside)
// confidence=low: steps the Axe palette-fade state machine in the context and selects a palette bank when one is queued.
void sub_080FB4EC(struct S080FB4EC_Actor *a)
{
    struct S080FB4EC_Ctx *ctx;
    u8 cur;
    s32 sv;
    ctx = a->context;
    if (ctx->unkED0) {
        a->drawable->unk32 = 0x13F;
        a->drawable->unk38 = (u8 *)ctx + 0x908;
        ctx->unkECF = 0;
        ctx->unkECE = 0;
        ctx->unkED0 = 0;
    }
    if (ctx->unkED1) {
        void *drawable = a->drawable->unk0 + 0x2C;
        u16 *palette = &a->unk1E0;
        u16 *offset = &a->unk1E6;
        AnimData_SelectPaletteBank(drawable, *palette + *offset);
        ctx->unkED1 = 0;
        return;
    }
    if (a->drawable->unk32 != 0x13F)
        return;
    switch ((s8)ctx->unkECF) {
        case 0:
        case 6:
            ctx->unk90A = 0;
            break;
        case 1:
        case 5:
            ctx->unk90A = 0x10;
            break;
        case 2:
        case 4:
            ctx->unk90A = 0x18;
            break;
        case 3:
            ctx->unk90A = 0x1F;
            break;
    }
    cur = ++ctx->unkECE;
    if (cur > 9) {
        u8 raw = ctx->unkECF;
        if ((s8)ctx->unkECF > 5)
            ctx->unkECF = 0;
        else
            ctx->unkECF = raw + 1;
        ctx->unkECE = 0;
    }
}
