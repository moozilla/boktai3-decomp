#include "vorn.h"
#include "scene_actor_reset.h"
static inline u8 Test(struct Vorn *actor, u32 mask)
{
    if (actor->flags6D4 & mask)
        return 1;
    return 0;
}
// confidence=low: reports whether a response flag or nonnegative response distance is present.
u8 Vorn_HasActiveResponse(struct Vorn *context)
{
    if (!Test(context, 1) && context->response < 0)
        return 0;
    return 1;
}
