#include "route_sprite_actor.h"
void RouteSpriteActor_RunEventScript(struct RouteSpriteActor *actor)
{
    u32 script = actor->script;
    if (script)
        Script_Call(script, 0);
}
