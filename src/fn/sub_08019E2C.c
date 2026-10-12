#include "global.h"
#define SpritePool_Remove sub_08019E2C
typedef struct {u32 id;u8 active,pad[7];u8 sprite[44];} Slot;
extern void *gUnk_020000B0;
Slot *sub_08019B50(void *,u32);
void sub_08214514(void *);
void SpritePool_Remove(u32 id) {
 if(gUnk_020000B0) {
  Slot *p=sub_08019B50(gUnk_020000B0,id);
  if(p) {sub_08214514(p->sprite);p->active=0;p->id=0;}
 }
}
