#include "monsters/screamer/screamer_state.h"

s32 SceneDrawablePair_SetFlags(struct ScreamerState *screamer)
{
    screamer->bodyCollider.flags |= 4;
    screamer->frontCollider.flags |= 4;
    screamer->sprite.flags |= 1;
    return 0;
}
