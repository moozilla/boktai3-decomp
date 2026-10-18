#include "global.h"
// Tentative: Turn toward the selected target by at most four angular units.
struct Vec {
    s16 x, y, z, pad;
};
struct Object {
    u8 pad0[9];
    u8 angle;
    u8 pada[2];
    struct Vec pos;
    u8 pad14[0x124];
    u32 range;
    u8 arc, pad13d[3];
    u32 distance;
    u8 delta;
    s8 target;
    u8 count, slot;
};
void *ActorPool_GetManager(void);
struct Object *Actor_GetPartyTarget(int);
int Math_Atan2(int, int);
void Actor_SetFacingAngle(struct Object *, int);
void Actor_StepFacingAngle(struct Object *p)
{
    if (p->target >= 0) {
        s8 *deltaPtr = (s8 *)(&p->delta);
        int delta;
        int angle;
        ;
        if ((*deltaPtr) > 3) {
            angle = p->angle + 4;
        } else if ((*deltaPtr) <= (-4)) {
            angle = p->angle - 4;
        } else {
            angle = (*deltaPtr) + p->angle;
        }
        Actor_SetFacingAngle(p, angle);
    }
}
