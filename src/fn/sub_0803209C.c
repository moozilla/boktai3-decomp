// MATCHING_CONCESSION: register-pinning
#include "global.h"
int sub_08031FEC(int *, int);
static inline int units(int *value) {
  int n = 0;
  int v = *value;
  u8 *dummy;
  if (v > 0) {
    int next;
    do {
      n++;
      next = v - 1;
      v = next;
    } while (next > 0);
    *value = next;
  }
  return n;
}
void sub_0803209C(u8 *buffer, volatile int value) {
  int started = 0;
  u8 *out = buffer;
  int digit;
  int zero = 0;
  u8 *q = buffer + 11;
  do {
    *q = zero;
    q--;
  } while ((s32)q >= (s32)buffer);
  if (value < 0) {
    *out = '-';
    out++;
    value = -value;
  }
  digit = sub_08031FEC(&value, 100000);
  if (digit) {
    *out = digit + '0';
    started = 1;
    out++;
  }
  digit = sub_08031FEC(&value, 10000);
  if (digit || started) {
    *out = digit + '0';
    started = 1;
    out++;
  }
  digit = sub_08031FEC(&value, 1000);
  if (digit || started) {
    *out = digit + '0';
    started = 1;
    out++;
  }
  digit = sub_08031FEC(&value, 100);
  if (digit || started) {
    *out = digit + '0';
    started = 1;
    out++;
  }
  digit = sub_08031FEC(&value, 10);
  if (digit || started) {
    *out = digit + '0';
    out++;
  }
  {
    int v;
    int next;
    u8 *end;
    digit = 0;
    v = value;
    end = out + 1;
    if (v > 0) {
      do {
        digit++;
        next = v - 1;
        v = next;
      } while (next > 0);
      value = next;
    }
    *out = digit + '0';
    *end = 0;
  }
}
