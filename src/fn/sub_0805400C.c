#include "saved_position_marker.h"
#define SavedPositionMarker_Destroy sub_0805400C
u32 SavedPositionMarker_Destroy(struct SavedPositionMarker *marker)
{
    Sprite_Remove(&marker->primary.sprite);
    Sprite_Remove(&marker->secondary.sprite);
    return 0;
}
