#include "scene_actor_record.h"
#define SceneActorRecord_CanRecordPair sub_0806FAB0
u32 SceneActorRecord_CanRecordPair(u16 key,u16 id)
{
 if(!sub_0806F9D8(key,id) && sub_0806F9AC())return 1;
 return 0;
}
