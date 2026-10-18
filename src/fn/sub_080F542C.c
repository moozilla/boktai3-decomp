#define ActorPhase_AccelerateTurn sub_080F542C
#include "global.h"
struct PhaseParent { u8 unknown0[0xD80]; u32 flags; };
struct PhaseActor {
 u8 unknown0[6]; u8 angle; u8 unknown7[0x159]; struct PhaseParent *parent;
 u8 unknown164[0x14]; void (*update178)(void); u8 unknown17C[8]; s32 timer;
 u8 targetAngle, unknown189; s8 direction, turnSteps;
};
void Sword_UpdateThrustDelay(void);
void sub_080F542C(struct PhaseActor *p) {
 struct PhaseParent *parent = p->parent;
 s32 i;
 for (i = 0; i < p->turnSteps; i++) {
  p->angle += (u8)p->direction;
  if (p->angle == p->targetAngle) {
   p->timer = 30;
   p->update178 = Sword_UpdateThrustDelay;
   return;
  }
 }
 if (!(parent->flags & 7)) {
  if (++p->turnSteps > 6) p->turnSteps = 7;
 }
}
