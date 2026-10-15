#include "scene_effect_schedule.h"
#define SceneEffectSchedule_RemoveFromScript sub_0806F768
struct SceneEffectScheduleEntry *sub_0806F3A8(u32);
void SceneEffectSchedule_RemoveFromScript(void)
{
 struct SceneEffectSchedule *manager=gUnk_0200019C;
 s32 id;
 struct SceneEffectScheduleEntry *entry;
 if(!manager)return;
 id=Script_GetKeywordValue('n',-1);
 if(id<0)return;
 entry=sub_0806F3A8(id);
 if(!entry)return;
 {s32 index=entry->index;
 u32 *mask;
 mask=&manager->occupiedMask;
 *mask &= ~(1u<<index);}

 Memory_Clear(entry,sizeof(*entry));
}
