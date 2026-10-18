#include "global.h"
#define ActorResource_ClampDecrease sub_08179F1C
struct ActorResourceState {
    u8 unknown0[0x94];
    s16 current;
    u8 unknown96[0x1DE];
    u16 limits[2];
    u8 unknown278[0x272];
    u8 selection;
};
s32 sub_08179F1C(struct ActorResourceState *p, s32 decrease)
{
    s32 result;
    s32 current = p->current;
    s32 next = current - decrease;
    if (p->selection == 0) {
        s32 limit = p->limits[0];
        if (next < limit) {
            result = current + 1;
            result -= limit;
            return result;
        }
    } else if (p->selection == 1) {
        s32 limit = p->limits[1];
        if (next < limit) {
            result = current + 1;
            result -= limit;
            return result;
        }
    }
    return decrease;
}
