// MATCHING_CONCESSION: register-pinning
#include "scene_actor_record.h"
#define SceneActorRecord_FindPair sub_0806F9D8
struct SceneActorRecord *SceneActorRecord_FindPair(u32 key,u32 id)
{
 s32 i=0;
 register u8 *base asm("r0")=(u8*)gUnk_02000710;
 u32 offset=0x670;
 register struct SceneActorRecord *entry asm("r2");
 struct SceneActorRecord *result;
 entry=(void*)(base+offset);
 result=entry;
 do {
  if(entry->key==key && entry->id==id)return result;
  entry++;result++;i++;
 }while(i<=19);
 return 0;
}
