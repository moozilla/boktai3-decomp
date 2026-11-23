#include "vanargand_pair_attack.h"

void Vanargand_UpdateProximityTransition(void *object)
{
    struct VanargandPairAttack *p = object;
    s32 dx, dz;
    struct SceneVec *destination, *position;
    if (Vanargand_ConsumeActionEntry(p)) {
        u32 alternate = ((struct VanargandPairAttack *)object)->variant;
        if (!alternate) {
            u32 phase = 4;
            p->actionPhase = phase;
            p->actionTimer = alternate;
            p->entered = 1;
            return;
        } else {
            Vanargand_EnterAction(p, 19);
        }
    }
    destination = p->destination;
    position = &p->position;
    dx = destination->x - position->x;
    dz = destination->z - position->z;
    if (dx * dx + dz * dz <= 65535) {
        Vanargand_EnterActionPhase(p, 16);
        Vanargand_SetActionAnimation(p, 17);
    } else if (p->finished) {
        Vanargand_EnterActionPhase(p, 13);
    }
    p->actionTimer++;
}
