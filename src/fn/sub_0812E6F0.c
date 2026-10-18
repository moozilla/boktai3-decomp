#include "global.h"
struct ActorModeState {
    u8 unknown0[0x1E6];
    u16 state;
};
u32 ActorMode_SelectOddStateVariant(struct ActorModeState *actor)
{
    u32 result;
    switch (actor->state) {
        case 1:
        case 3:
        case 5:
            result = 3;
            break;
        case 0:
        default:
            result = 4;
            break;
    }
    return result;
}
