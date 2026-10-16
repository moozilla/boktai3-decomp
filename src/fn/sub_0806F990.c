#include "scene_actor_record.h"
void SceneActorRecord_Clear(void)
{
    Memory_Clear((u8 *)SCENE_ACTOR_RECORDS, 0x80);
}
