#define CannonBattery_UpdateProjectile sub_08199E68
#include "global.h"
#include "engine.h"
struct CannonBatteryProjectile {
    void *update, *destroy;
    u8 unk8[0x38];
    s16 x, y, z, unk46;
    u8 unk48[0x18];
    u8 collider[0x54];
    u16 startCoordinate;
    u8 direction;
};
void sub_08199E68(struct CannonBatteryProjectile *p)
{
    s16 *position;
    switch (p->direction) {
        case 0:
            p->z -= 0x18;
            if (p->startCoordinate - p->z > 0x1000) {
                p->update = NULL;
                if (p->destroy) {
                    Callback_InvokeActor(p, p->destroy);
                    p->destroy = NULL;
                }
            }
            break;
        case 0x10:
            p->x -= 0x18;
            if (p->startCoordinate - p->x > 0x1000) {
                p->update = NULL;
                if (p->destroy) {
                    Callback_InvokeActor(p, p->destroy);
                    p->destroy = NULL;
                }
            }
            break;
    }
    position = &p->x;
    Collider_Refresh(p->collider);
    Collider_SetPosition(p->collider, position, 0);
}
