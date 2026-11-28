#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_UpdateProximityTransition(void *object)
{
    struct VanargandPairAttack *p = object;
    s32 dx, dz;
    struct SceneVec *destination, *position;
    if (Vanargand_ConsumeActionEntry(p)) {
        if (!((struct VanargandPairAttack *)object)->variant) {
            Vanargand_EnterActionPhase(p, 4);
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
