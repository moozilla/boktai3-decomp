#include "global.h"
#include "player.h"
#include "directional_interaction.h"

#define ControlPanel_UpdateMultiplierDisplay sub_081C1E48

struct E {
    u32 fl;
    u8 pad4[0xC];
    u16 a10;
    u8 pad12[0xC];
    u16 a1E;
    u8 pad20[0xC];
};

struct S {
    u8 f[0x250];
    struct E e[6];
    u8 pad358[0x590 - 0x358];
    u16 multiplier;
    u8 pad592[0x5A4 - 0x592];
    u32 field5A4;
    u8 pad5A8[0x5B0 - 0x5A8];
    u16 pendingValue;
    u16 cap;
};

u32 ControlPanel_GetInputMultiplier(struct S *p);
void sub_081C1B20(struct S *p);

s32 sub_081C1E48(struct S *p)
{
    struct Player *player = gUnk_02000580[0];
    s32 extra;
    u32 multiplier;
    s32 count;

    if (player->displaySegmentCount == 0) {
        extra = 0;
    } else if (player->displaySegmentCount <= 3) {
        extra = 1;
    } else {
        s32 x = player->displaySegmentCount;
        extra = ((x - 2) * (x - 3)) >> 1;
    }

    multiplier = ControlPanel_GetInputMultiplier(p);

    if (p->multiplier != multiplier) {
        if (p->field5A4 != 0) {
            struct DirectionalScriptArgs args;
            s32 value;
            args.count = 1;
            args.values = &value;
            value = multiplier;
            Script_Call(p->field5A4, &args);
        }
        p->multiplier = multiplier;
        sub_081C1B20(p);
    }

    {
        s32 sum = extra * p->multiplier + p->pendingValue;
        u16 *pendingPtr = &p->pendingValue;
        u16 *capPtr = &p->cap;

        *pendingPtr = sum;

        if ((u16)sum < *capPtr)
            return 0;

        count = 0;
        if ((u16)sum < *capPtr)
            return count;
        do {
            count++;
            *pendingPtr -= *capPtr;
        } while ((u16)*pendingPtr >= *capPtr);
    }
    return count;
}
