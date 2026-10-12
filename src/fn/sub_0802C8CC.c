// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Object {
  u8 pad0[9];
  u8 angle;
  u8 pada[0x42];
  int health;
  u32 flags;
  u8 pad54[4];
  int damage;
  u32 source, attackFlags;
  u8 pad64[2];
  u16 reaction;
  u8 pad68[0x118];
  u8 facing, facing8;
  u8 pad182[0xc6];
  int maxHealth;
};
struct Collider {
  u8 pad0[0x34];
  u32 source, flags;
  u8 pad3c[2];
  u16 damage;
  u8 pad40[4];
  u16 reaction;
};
void sub_0802C8CC(struct Object *p, int angle) {
  p->angle = angle;
  {
    register int converted asm("r1") = (u16)angle;
    register u32 value asm("r2");
    asm("" : "+r"(converted));
    {
      register int shifted asm("r0") = converted + 112;
      asm("" : "+r"(shifted));
      shifted >>= 5;
      asm("" : "+r"(shifted));
      value = 7 & shifted;
    }
    asm("" : "+r"(value));
    p->facing8 = value;
    converted += 96;
    asm("" : "+r"(converted));
    converted >>= 6;
    asm("" : "+r"(converted));
    {
      register u32 value2 asm("r0") = 3 & converted;
      asm("" : "+r"(value2));
      p->facing = value2;
    }
  }
}
