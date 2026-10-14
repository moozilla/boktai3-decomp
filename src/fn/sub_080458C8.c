#define MotionPool_InitializeParams       sub_080458C8
#define MotionPool_SetLifetimeAndVelocity sub_08045934
#define MotionPool_SetContact             sub_08045950
#define MotionPool_SetBounds              sub_08045968
#define MotionPool_SetOffset              sub_08045970
#define MotionPool_SetGraphics            sub_08045978
#define MotionPool_SetCallback            sub_08045980
#define MotionPool_UpdateUntilDeadline    sub_080459F8
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
