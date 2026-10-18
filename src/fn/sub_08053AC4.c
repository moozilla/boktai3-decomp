#include "interaction_marker.h"
void SceneBody_Unregister(void *);
u32 InteractionMarker_Destroy(struct InteractionMarker *marker)
{
    Sprite_Remove(&marker->primary.sprite);
    Sprite_Remove(&marker->secondary.sprite);
    if (marker->bodyEnabled != 0)
        SceneBody_Unregister(marker->body);
    return 0;
}
