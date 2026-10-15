#include "route_sprite_actor.h"
#define RouteSpriteActor_RunEventScript sub_0806CFE4
void RouteSpriteActor_RunEventScript(struct RouteSpriteActor *actor)
{
    u32 script = actor->script;
    if (script)
        Script_Call(script, 0);
}
