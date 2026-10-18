#include "global.h"

/* Tentative angle gate: the two modes select overlapping half-circle ranges. */
struct DirectionGate {
    u8 unknown0[0xCC];
    u8 angle;
    u8 unknownCD[0x25E];
    u8 mode;
};
u32 DirectionGate_IsAngleInModeRange(struct DirectionGate *gate)
{
    u32 angle = gate->angle;
    switch (gate->mode) {
        case 0:
            if (angle - 0x41 <= 0x7E)
                return 1;
            break;
        case 1:
            if (angle - 0x81 <= 0x7E)
                return 1;
            break;
    }
    return 0;
}
