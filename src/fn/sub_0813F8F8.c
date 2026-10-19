
#ifdef NONMATCHING
// fuzzy: 85.0%
// foundry draft (worker best 85.0%); what differs is mostly register allocation and instruction order
#define Player_SelectRequestedTetherMode sub_0813F8F8
#include "global.h"
#include "player.h"
extern u32 gUnk_030053F4;
void Sound_PlaySong(u32);
s32 sub_0813F8F8(struct Player *p)
{
 struct Player *actor = p;
 struct PlayerActionRequest *request = (struct PlayerActionRequest *)&p->thresholdMode;
 if (request->kind >= 0 && request->enabled && !request->blocked) {
  if (actor->tetherMode > 1) return actor->tetherMode;
  switch (request->kind) {
   case 3:
    if (gUnk_030053F4 & 0x1000) return actor->tetherMode;
    return 19;
   case 2:
    if (actor->actionRequestRestricted == 1) {
     Sound_PlaySong(402);
     return actor->tetherMode;
    }
    if (gUnk_030053F4 & 0x1000) return actor->tetherMode;
    return 10;
   case 1:
    if (actor->actionRequestRestricted == 1) {
     Sound_PlaySong(402);
     return actor->tetherMode;
    }
    if (gUnk_030053F4 & 0x1000) return actor->tetherMode;
    return 11;
   case 0:
    if (actor->actionMode == 0 || actor->actionMode == 6) return 2;
    return actor->tetherMode;
  }
 }
 return actor->tetherMode;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0813F8F8);
#endif
