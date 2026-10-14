#define MotionPool_SetContact sub_08045950
#include "pooled_motion.h"
void MotionPool_SetContact(struct MotionPoolParams *params, u32 mode, u32 flags, u32 id, u32 cost,
                           u32 option)
{
    params->contactMode = mode;
    params->contactFlags = flags;
    params->resourceId = id;
    params->cost = cost;
    params->option = option;
}
