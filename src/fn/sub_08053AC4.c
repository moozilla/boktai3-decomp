#include "interaction_marker.h"
void sub_08225938(void *);
u32 InteractionMarker_Destroy(struct InteractionMarker *marker)
{
    Sprite_Remove(&marker->primary.sprite);
    Sprite_Remove(&marker->secondary.sprite);
    if (marker->bodyEnabled != 0)
        sub_08225938(marker->body);
    return 0;
}
