#include "global.h"
// MATCHING_CONCESSION: register-pinning
void *ActorPool_AllocObject(int);
void Actor_SetLifecycleCallbacks(void *, void (*)(void *), void (*)(void *));
void Actor_OnControllerUpdate(void *);
void Actor_OnControllerDestroy(void *);
int Actor_InitializeControllerFromScript(void *);
void Actor_RequestRemoval(void *);
void *Actor_CreateControllerFromScript(void)
{
    register void *p asm("r4") = ActorPool_AllocObject(0x220);
    void *allocated = p;
    asm("" : "+r"(p));
    if (p) {
        Actor_SetLifecycleCallbacks(p, Actor_OnControllerUpdate, Actor_OnControllerDestroy);
        if (Actor_InitializeControllerFromScript(p) >= 0)
            return p;
        Actor_RequestRemoval(allocated);
    }
    return 0;
}
