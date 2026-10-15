#include "scene_actor_record.h"
#define SceneActorRecord_RecordPair sub_0806FA74
void SceneActorRecord_RecordPair(u32 keyWord,u32 idWord)
{
 u16 key=keyWord,id=idWord;
 struct SceneActorRecord *entry;
 struct SceneActorRegistryManager *manager;
 entry=sub_0806F9D8(key,id);
 if(entry)return;
 entry=sub_0806F9AC();
 if(!entry)return;
 manager=sub_08072B44();
 if(!manager)return;
 entry->key=key;entry->id=id;entry->ticks=manager->mode;
}
