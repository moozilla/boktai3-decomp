#include "engine/shared/global.h"
#include "link/registry_1745/room_actor.h"
void sub_08249240(void *, const u16 *, SceneActorCommandHandler);
/* The shared onMessage: a script command's first halfword picks the handler in the actor's
 * command table. */
void SceneActorScript_OnCommand(void *object, const u16 *command)
{
    struct RoomActor *actor = object;
    if (command != 0)
        sub_08249240(actor, command, actor->commandTable.handlers[command[0]]);
}
