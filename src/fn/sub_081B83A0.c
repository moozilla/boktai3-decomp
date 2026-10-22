// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.6%
// Open: the seed address takes r7 for the first sample, so the 0x1f mask is rematerialized
// instead of shared in r7, and the parameter-pointer spill is set up later than in the ROM.
#include "scene_vec.h"
#define gRandomIndex gUnk_03005308
struct SceneScriptBurstObject {
    u8 unknown0[0x34];
    struct SceneVec position;
    u8 unknown3C[0xEC - 0x3C];
    s16 state;
    u8 unknownEE[8];
    s16 parameter;
    u8 unknownF8[0x13E - 0xF8];
    u16 counter;
};
extern u32 gUnk_03005308;
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void Task_Kill(void *);
static inline s32 Sample(u32 table, u32 index)
{
    return *(const u16 *)(index * 2 + table);
}
static inline s32 PaletteIndex(s32 state, s32 parameter)
{
    if (state <= 1) {
        if (parameter >= 0)
            return parameter + 0x26B;
        return 0x26B;
    }
    return ((state - 2) >> 1) + 0x26B;
}
void SceneScriptObject_UpdateBurst(struct SceneScriptBurstObject *object)
{
    struct SceneVec velocity;
    u32 index;
    u16 counter;
    if (object->counter == 0) {
        register s32 i asm("r6") = 0;
        s16 *statePtr = &object->state;
        struct SceneVec *position = &object->position;
        register u32 mask asm("r8") = 0x3ff;
        struct SceneVec *v = &velocity;
        s16 *parameterPtr = &object->parameter;
        register s32 zero asm("r5") = 0;
        do {
            s32 kind = (i & 1) ? 0x400 : 0x200;
            u32 table = RANDOM_TABLE_ADDR;
            s32 x, y, z;
            register u32 index asm("r1");
            s32 threshold;
            asm("" : "+r"(table) : "r"(index));
            index = (gRandomIndex + 1) & mask;
            x = (Sample(table, index) >> 4) & 0x1f;
            index = (index + 1) & mask;
            y = (Sample(table, index) >> 4) & 0x1f;
            index = (index + 1) & mask;
            z = (Sample(table, index) >> 4) & 0x1f;
            x -= 0x10;
            y += 0x10;
            z -= 0x10;
            v->x = x;
            v->y = y;
            v->z = z;
            index = (index + 1) & mask;
            gRandomIndex = index;
            threshold = ((Sample(table, index) >> 4) & 0x7f) + 0x20;
            sub_08127590(position, &velocity, 0x83A6, 0x99CB, *statePtr, threshold, kind, zero,
                         PaletteIndex(*statePtr, *parameterPtr), zero, i & 1, zero, zero, zero,
                         zero);
            i++;
        } while (i <= 3);
    }
    counter = object->counter;
    index = (gRandomIndex + 1) & 0x3ff;
    gRandomIndex = index;
    if (counter >= (s32)(((Sample(RANDOM_TABLE_ADDR, index) >> 4) & 0x7f) + 0x20))
        Task_Kill(object);
    else
        object->counter++;
}
#else
INCLUDE_ASM("asm/nonmatching", SceneScriptObject_UpdateBurst);
#endif
