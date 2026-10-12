#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
  s16 x, y, z, pad;
};
struct Actor {
  u8 active;
  u8 pad1[1];
  u8 stage;
  u8 phase;
  u8 state;
  u8 pad5[3];
  u8 entered;
  u8 pad9[2];
  u8 enabled;
  u8 padc[8];
  u32 ticks;
  u8 pad18[1];
  u8 facing4;
  u8 facing8;
  u8 pad1b[37];
  u32 script;
  u8 pad44[24];
  u32 body;
  u8 pad60[5];
  u8 angle;
  u8 pad66[2];
  struct Vec pos;
  u8 pad70[300];
  u8 path[8];
  void *map;
  u8 pad1a8[68];
  u8 modeA;
  u8 modeB;
  u16 modeValue;
  u8 pad1f0[1];
  u8 hasDestination;
  u8 pad1f2[1];
  s8 pathId;
  s16 radiusA;
  s16 radiusB;
  int radiusA32;
  int radiusB32;
  int radiusASquared;
  int radiusBSquared;
  int distance;
  int distanceSquared;
  u16 pathParamA;
  u8 pad212[2];
  u16 pathParamB;
  u8 soundEnabled;
  u8 pad217[2];
  u8 savedAngle;
  u8 registered;
  u8 controlled;
  int rateA;
  int rateB;
  u8 pad224[4];
  int rateC;
  u8 pad22c[4];
  u8 hasParameters;
  u8 paramA;
  u8 paramB;
  u8 paramC;
  u8 paramD;
  u8 pad235[3];
  u8 turnA;
  u8 turnB;
  u8 turnRate;
  u8 turnState;
  struct Vec destination;
};
extern void *gUnk_02000480;
struct Actor *sub_0802D6D0(void *, int);
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int sub_0802FD14(struct Actor *);
void sub_08225B4C(void *, void *, int, int, int);
void sub_0821E780(void *, void *, int);
int sub_0802E5E8(void) {
  void *m = gUnk_02000480;
  struct Actor *p;
  register int mode asm("r0");
  if (!m)
    return -1;
  p = sub_0802D6D0(m, sub_0821ABA8(0x6e, 0));
  if (!p)
    return -1;
  {
    u8 *controlled = &p->controlled;
    int zero;
    asm("" : "+r"(controlled));
    zero = 0;
    *controlled = zero;
    mode = sub_0821ABA8(0x69, 0);
    {
      u32 command = mode;
      switch (command) {
      default:
        break;
      case 0: {
        register int one asm("r1") = 1;
        p->stage = command;
        p->phase = command;
        p->state = one;
        p->ticks = command;
        p->entered = one;
        break;
      }
      case 1:
        p->stage = zero;
        p->phase = zero;
        p->state = zero;
        p->ticks = zero;
        p->entered = command;
        break;
      }
    }
  }
  return mode;
}
