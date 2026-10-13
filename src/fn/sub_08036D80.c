// MATCHING_CONCESSION: register-pinning
#include "global.h"
u16 *sub_08036CA4(void);
u16 sub_08036CAC(u8);
u8 sub_08036CC4(u16);
int Script_SeekToKeyword(int);
void *Script_GetPc(void);
int Script_ParseStringRef(void *);
const u8 *Text_LookupString(int);
void sub_08036D80(void) {
  register void *script asm("r6");
  u16 *table;
  register int n asm("r5");
  register int i asm("r4");
  register int capacity asm("r8");
  if (Script_SeekToKeyword(115))
    script = Script_GetPc();
  else
    script = 0;
  if (!script)
    return;
  table = sub_08036CA4();
  n = 0;
  i = 0;
  capacity = 256;
  asm("" : "+r"(capacity));
  do {
    register const u8 *text asm("r2") =
        Text_LookupString(Script_ParseStringRef(script) + i);
    register int first asm("r1") = *text;
    u16 *entry;
    i++;
    if (!first)
      continue;
    if (n == capacity)
      continue;
    entry = (u16 *)((n << 1) + (u32)table);
    do {
      register int sign asm("r0") = first << 24;
      asm("" : "+r"(sign) : : "memory");
      if (sign >= 0) {
        *entry = *text;
        text++;
      } else {
        register int high asm("r1") = text[1] << 8;
        register int code asm("r0") = text[0] | high;
        *entry = code;
        text += 2;
      }
      entry++;
      n++;
      first = *text;
      if (!first)
        break;
    } while (n != capacity);
  } while (i <= 4);
  table[0] = 0;
}