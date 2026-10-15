#include "saved_position_marker.h"
#define SavedPositionMarker_TickInteraction sub_08053F48
struct MarkerScriptArgs {
    u32 count : 16;
    u32 pad : 16;
    u32 *values;
};
void SavedPositionMarker_TickInteraction(struct SavedPositionMarker *marker)
{
    if (marker->height > 1)
        marker->height -= 2;
    else
        marker->height = 0;
    marker->primary.sprite.position.y = marker->position.y + marker->height;
    if (marker->height == 0 && gUnk_02000580[0]->action == 0) {
        marker->primary.sprite.flags &= ~0x200;
        marker->primary.sprite.priority = 3;
        marker->secondary.sprite.flags |= 1;
        sub_08053DE8(marker, sub_08053FF4);
        if (marker->scriptId) {
            u32 values[2];
            struct MarkerScriptArgs args;
            args.count = 2;
            args.values = values;
            values[0] = marker->scriptArgs[0];
            values[1] = marker->scriptArgs[1];
            Script_Call(marker->scriptId, &args);
        }
    }
}
