#include "objects/markers/interaction_marker.h"
#include "script/script_call_arguments_layout.h"
void InteractionMarker_TickInteraction(struct InteractionMarker *marker)
{
    if (marker->height > 1)
        marker->height -= 2;
    else
        marker->height = 0;
    marker->primary.sprite.position.y = marker->position.y + marker->height;
    if (marker->height == 0 && gPlayers[0]->tetherMode == 0) {
        marker->primary.sprite.flags &= ~0x200;
        marker->primary.sprite.priority = 3;
        marker->secondary.sprite.flags |= 1;
        InteractionMarker_SetState(marker, InteractionMarker_TickDormant);
        if (marker->scriptId) {
            u32 values[2];
            struct ScriptCallArguments args;
            args.count = 2;
            args.values = values;
            values[0] = marker->scriptArgs[0];
            values[1] = marker->scriptArgs[1];
            Script_Call(marker->scriptId, &args);
        }
    }
}
