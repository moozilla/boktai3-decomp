#define ActorPhase_TurnToHalfCircle sub_080F5620
#include "global.h"
struct PhaseParent { u8 unknown0[0xD80]; u32 flags; };
struct PhaseActor {
 u8 unknown0[6]; u8 angle; u8 unknown7[0x112]; u8 flags119;
 u8 unknown11A[4]; u8 field11E, field11F; u8 unknown120[0x40]; struct PhaseParent *parent;
 void (*update164)(void); u8 unknown168[0x22]; s8 direction, turnSteps;
};
void sub_080F60B8(void);
void sub_080F5620(struct PhaseActor *p) {
 struct PhaseParent *parent = p->parent;
 s32 i;
 for (i = 0; i < p->turnSteps; i++) {
  p->angle += (u8)p->direction;
  if (p->angle == 128) {
   u16 one = 1;
   p->field11E = one;
   p->field11F = one;
   p->flags119 = one | p->flags119;
   p->update164 = sub_080F60B8;
   return;
  }
 }
 if (!(parent->flags & 7)) {
  if (++p->turnSteps > 6) p->turnSteps = 7;
 }
}
