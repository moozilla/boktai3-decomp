#include "tether_actor.h"
#define TetherActor_TestStepDelta sub_0804BE20
u32 sub_0804BCD4(struct ContactPosition *, struct ContactPosition *, u32);
u32 TetherActor_TestStepDelta(struct ContactPackedVector *origin, struct ContactPackedVector *delta, u32 mask)
{
    struct ContactPackedVector candidate;
    if (!delta->x) {
        candidate = *origin;
        candidate.z += delta->z;
        if (sub_0804BCD4((void *)origin, (void *)&candidate, mask))
            goto yes;
        goto no;
    } else if (!delta->z) {
        candidate = *origin;
        *(u16 *)&candidate += (u16)delta->x;
        if (sub_0804BCD4((void *)origin, (void *)&candidate, mask))
            goto yes;
        goto no;
    } else {
        candidate = *origin;
        *(u16 *)&candidate += (u16)delta->x;
        if (!sub_0804BCD4((void *)origin, (void *)&candidate, mask))
            goto no;
        candidate = *origin;
        ((u16 *)&candidate)[2] += (u16)delta->z;
        if (!sub_0804BCD4((void *)origin, (void *)&candidate, mask))
            goto no;
        *(u16 *)&candidate += (u16)delta->x;
    }
    if (sub_0804BCD4((void *)origin, (void *)&candidate, mask))
        goto yes;
no:
    return 0;
yes:
    return 1;
}
