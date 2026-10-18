#include "global.h"
struct ActorMarker {
    u8 pad0[0xd4];
    u16 position[3];
    u8 padDA[0x3c6 - 0xda];
    u16 lifetime;
    u8 pad3C8[0x3de - 0x3c8];
    u8 state;
    u8 pad3DF[0x3f4 - 0x3df];
    u16 spritePosition[3];
    u8 pad3FA[0x404 - 0x3fa];
    u16 velocityZ, counter, velocityX;
    u8 pad40A[0xb38 - 0x40a];
    u8 kind, direction;
};
void ActorMarker_InitFacingSprite(struct ActorMarker *);
void ActorMarker_InitMovingSprite(struct ActorMarker *p)
{
    switch (p->kind) {
        default:
            ActorMarker_InitFacingSprite(p);
            p->velocityX = 0;
            p->counter = 0;
            p->velocityZ = 0;
            p->lifetime = 30;
            p->spritePosition[0] = p->position[0];
            p->spritePosition[1] = 400 + p->position[1];
            p->spritePosition[2] = p->position[2];
            p->state = 0;
            switch (((p->direction + 0x60) >> 6) & 3) {
                case 0:
                    p->velocityX = 15;
                    break;
                case 1:
                    p->velocityZ = 0xfff1;
                    break;
                case 2:
                    p->velocityX = 0xfff1;
                    break;
                case 3:
                    p->velocityZ = 15;
                    break;
            }
            break;
        case 3:
        case 5:
        case 7:
        case 9:
        case 13:
        case 15:
            break;
    }
}
