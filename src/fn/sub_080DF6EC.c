#include "global.h"

#include "scene_actor_reset.h"

// confidence=low: Dog step gated on engine flags and context state; picks a random slot, sets the sound variant from health, and records the target distance and heading.

// confidence=low: Dog step gated on engine flags and context state; picks a random slot, sets the sound variant from health, and records the target distance and heading.

// confidence=low: Dog step gated on engine flags and context state; picks a random slot, sets the sound variant from health, and records the target distance and heading.

// confidence=low: Dog step gated on engine flags and context state; picks a random slot, sets the sound variant from health, and records the target distance and heading.

extern u32 gUnk_030053F4;
extern u8 *gUnk_02000580[4];
s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
u8 InventorySlot_PickRandom(void *, void *, void *);
void Inventory_ClearItemSlot(s32);
s32 sub_08017044(s32, s32, s32, s32, void *, void *, s32, void *);
static inline u8 F6ECTest(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
#ifdef NONMATCHING
// fuzzy: 64.7%
// scope shape 36 (differences once low registers are set aside)
// confidence=low: Dog step gated on engine flags and context state; picks a random slot, sets the sound variant from health, and records the target distance and heading.
void sub_080DF6EC(s32 unused, u8 *b, struct SceneActorRegistryResetView *a)
{
    u8 *context;
    struct ContactPosition pos;
    u8 *obj;
    s32 dx, dz;
    s32 amount;
    s16 kind;
    u16 mode;
    u32 mask;

    mask = 0xFFFF;
    if (F6ECTest(gUnk_030053F4, 0x4800))
        return;
    if (*(u16 *)(b + 0xA) != 1)
        return;
    context = a->context;
    mode = a->paletteOffset;
    if (*(s16 *)(context + 0x6A0) >= 0)
        return;
    if (mode != 0 && mode != 2)
        return;
    pos = *(struct ContactPosition *)(gUnk_02000580[0] + 0x30);
    pos.y += 200;
    if (!InventorySlot_PickRandom(context + 0x6A0, context + 0x6A2, context + 0x6A4))
        return;
    if (sub_08017044(0, *(s16 *)(context + 0x6A2), 3, 0, &pos, &a->position, 0x18, a) < 0) {
        *(u16 *)(context + 0x6A0) |= mask;
        return;
    }
    Inventory_ClearItemSlot(*(s16 *)(context + 0x6A0));
    kind = *(s16 *)(context + 0x6A2);
    switch (kind) {
        case 2:
        case 5:
        case 0xE:
            break;
        default:
            return;
    }
    amount = Div((u16)a->defaultHealth * context[0x6A9], 100);
    if (a->health - amount > 0)
        a->reactionSoundVariant = amount;
    else
        a->reactionSoundVariant = a->health;
    *(u16 *)(context + 0x6A0) = 0xFFFF;
    a->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
    a->field2E0 = 1;
    a->field2D8 = 0;
    a->field2DC = 0;
    a->field2D8 = 0;
    a->field2DC = 0;
    *(u16 *)((u8 *)a + 0x172) = a->reactionSoundVariant;
    *(u16 *)((u8 *)a + 0x174) = 1;
    *(u16 *)((u8 *)a + 0x178) = 0x1E;
    a->field2E6 = 1;
    if (F6ECTest(a->flags2C0, 2))
        a->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
    else if (a->fieldDC == 0)
        return;
    obj = (u8 *)a->fieldDC;
    dx = *(s16 *)(obj + 0xC) - a->position.x;
    dz = *(s16 *)(obj + 0x10) - a->position.z;
    a->fieldE9 = Math_Atan2(dx, dz);
    a->distanceF0 = dx * dx + dz * dz;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080DF6EC);
#endif
