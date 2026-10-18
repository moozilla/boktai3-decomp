#include "global.h"
#define ActorFlags_SelectResponse sub_0813B480
#define ActorFlags_GetResponseBits sub_0814A854
#define ActorResponse_Apply sub_0813B418
s32 sub_0814A854(void *);
s32 sub_0813B418(void *, s32);
s32 sub_0813B480(void *actor)
{
    s32 flags = sub_0814A854(actor);
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
    response = sub_0813B418(actor, response);
    return response;
}
