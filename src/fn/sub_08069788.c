#include "bouncing_object.h"
/* BouncingObject_IsActive: Returns true for a nonnull object with a nonnegative slot index and nonzero timer whose state is not 2 */
#define BouncingObject_IsActive sub_08069788
u32 BouncingObject_IsActive(struct BouncingObject *input)
{
    struct BouncingObject *object = input;

    if (!input)
        return 0;
    if (input->index < 0)
        return 0;
    if (!input->timer)
        return 0;
    if (object->state == 2)
        return 0;
    return 1;
}
