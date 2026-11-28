#include "global.h"
#include "engine/engine.h"
#include "objects/deck_cannons/script_value_rotation_layout.h"

u32 BackgroundAnimation_Destroy(struct ScriptValueRotation *rotation)
{
    Anim_Remove(rotation->animation);
    return 0;
}
