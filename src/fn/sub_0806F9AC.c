#include "scene_actor_record.h"
#define SceneActorRecord_FindEmpty sub_0806F9AC
struct SceneActorRecord *SceneActorRecord_FindEmpty(void)
{
 s32 i=0;struct SceneActorRecord *entry=SCENE_ACTOR_RECORDS;
 struct SceneActorRecord *result;
 do {if(!entry->id){result=entry;goto done;}entry++;i++;}while(i<=19);
 result=0;
done:
 return result;
}
