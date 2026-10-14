#include "pooled_motion.h"
void MotionPool_InitializeParams(struct MotionPoolParams *params)
{
    Memory_Clear(params, sizeof(*params));
    MotionPool_SetLifetimeAndVelocity(params, 0, 60, 0, 0);
    MotionPool_SetContact(params, 0, 0, 10, 20, 20);
    MotionPool_SetBounds(params, 8, 32, 8);
    MotionPool_SetOffset(params, 0, 0, 0);
    MotionPool_SetGraphics(params, 0x848F, 0, -1);
    MotionPool_SetCallback(params, MotionPool_UpdateUntilDeadline);
}
