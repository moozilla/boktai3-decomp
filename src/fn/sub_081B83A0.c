#include "global.h"
#include "scene_vec.h"
#include "ram.h"
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
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void Task_Kill(void *);
static inline s32 Sample(u32 table, u32 index)
{
    return *(const u16 *)(index * 2 + table);
}
static inline s32 AdvanceAndSample(u32 *table, u32 index)
{
    *table += index * 2;
    return *(const u16 *)*table;
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
        s32 i = 0;
        s16 *statePtr;
        struct SceneVec *position;
        u32 mask;
        struct SceneVec *v;
        s16 *parameterPtr;
        s32 zero;
        statePtr = &object->state;
        parameterPtr = &object->parameter;
        position = &object->position;
        mask = 0x3ff;
        v = &velocity;
        zero = 0;
        do {
            s32 kind = (i & 1) ? (0x400) : (0x200);
            u32 table = 0x0203B400;
            s32 x;
            s32 y;
            s32 z;
            u16 index;
            s32 threshold;
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
            threshold = ((AdvanceAndSample(&table, index) >> 4) & 0x7f) + 0x20;
            sub_08127590(position, &velocity, 0x83A6, 0x99CB, *statePtr, threshold, kind, zero,
                         PaletteIndex(*statePtr, *parameterPtr), zero, i & 1, zero, zero, zero,
                         zero);
            i++;
        } while (i <= 3);
    }
    counter = object->counter;
    index = (gRandomIndex + 1) & 0x3ff;
    gRandomIndex = index;
    if (counter >= ((s32)(((Sample(0x0203B400, index) >> 4) & 0x7f) + 0x20)))
        Task_Kill(object);
    else
        object->counter++;
}
