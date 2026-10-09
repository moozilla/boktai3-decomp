#include "global.h"
s32 sub_08061120(u8 *);
void sub_0822B2F8(u32);
void sub_0806167C(u8 *);
void sub_08061780(u8 *);
void sub_08061628(u8 *);
void sub_08061914(u8 *);
void sub_08061F18(u8 *s) {
 switch(s[0x1AA6]) {
 case 4:
 case 5:
  switch(sub_08061120(s)) {
  case 0: goto error;
  case 1: sub_0822B2F8(0xDD); sub_0806167C(s); break;
  case -1: sub_0822B2F8(0xDD); sub_08061780(s); break;
  }
  break;
 case 1: sub_0822B2F8(0xDD); sub_08061628(s); break;
 case 2:
  if(sub_08061120(s)==0) {
error:
   sub_0822B2F8(0x192);
  } else {
   sub_0822B2F8(0xDD); sub_08061914(s);
  }
  break;
 }
}
