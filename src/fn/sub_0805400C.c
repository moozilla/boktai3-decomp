#include "saved_position_marker.h"
u32 SavedPositionMarker_Destroy(struct SavedPositionMarker *marker)
{
    Sprite_Remove(&marker->primary.sprite);
    Sprite_Remove(&marker->secondary.sprite);
    return 0;
}
