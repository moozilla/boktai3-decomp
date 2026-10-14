#include "tether_actor.h"
u32 TetherActor_TestStepPosition(struct ContactPosition *, struct ContactPosition *, u32);
u32 TetherActor_TestStepDelta(struct ContactPackedVector *origin, struct ContactPackedVector *delta,
                              u32 mask)
{
    struct ContactPackedVector candidate;
    if (!delta->x) {
        candidate = *origin;
        candidate.z += delta->z;
        if (TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
            goto yes;
        goto no;
    } else if (!delta->z) {
        candidate = *origin;
        *(u16 *)&candidate += (u16)delta->x;
        if (TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
            goto yes;
        goto no;
    } else {
        candidate = *origin;
        *(u16 *)&candidate += (u16)delta->x;
        if (!TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
            goto no;
        candidate = *origin;
        ((u16 *)&candidate)[2] += (u16)delta->z;
        if (!TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
            goto no;
        *(u16 *)&candidate += (u16)delta->x;
    }
    if (TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
        goto yes;
no:
    return 0;
yes:
    return 1;
}
