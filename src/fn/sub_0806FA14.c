// MATCHING_CONCESSION: register-pinning
#include "scene_actor_record.h"
#define SceneActorRecord_Tick sub_0806FA14
static inline u32 Enabled(void)
{
 u32 mask=4;
 {struct GameState *base=gUnk_02000710;
 u32 result;
 if(base->flags868&mask){result=1;asm("" : "+r"(result));}
 else {result=0;asm("" : "+r"(result));}
 return result;}

}
void SceneActorRecord_Tick(void)
{
 u32 enabled=Enabled();
 if(enabled){
  register struct GameState **location asm("r5")=&gUnk_02000710;
  u32 offset;
  u32 tableOffset;
  s32 i;
  offset=0;
  tableOffset=0x670;
  i=19;
  do {
   struct SceneActorRecord *entry=(void*)((u8*)*location+(offset+tableOffset));
   if(entry->id){entry->ticks--;if(!entry->ticks)entry->id=0;}
   offset+=6;
  }while(--i>=0);
 }
}
