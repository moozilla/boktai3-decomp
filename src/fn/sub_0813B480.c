#include "global.h"
s32 ActorFlags_GetResponseBits(void *);
s32 ActorResponse_Apply(void *, s32);
s32 ActorFlags_SelectResponse(void *actor)
{
    s32 flags = ActorFlags_GetResponseBits(actor);
    s32 response;
    if (flags & 1)
        response = 5;
    else if (flags & 4)
        response = 10;
    else if (flags & 8)
        response = 10;
    else if (flags & 0x10)
        response = 10;
    else if (flags & 0x20)
        response = 10;
    else
        response = 0;
    response = ActorResponse_Apply(actor, response);
    return response;
}
