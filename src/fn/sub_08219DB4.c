#include "global.h"
#include "engine.h"
/* Memory_FillWords: Fills the requested number of 32-bit words with a supplied value using an unrolled loop */
#define Memory_FillWords sub_08219DB4

void sub_08219DB4(u32 *p, u32 v, s32 n)
{
    while (n > 3) {
        p[0] = v;
        p[1] = v;
        p[2] = v;
        p[3] = v;
        p += 4;
        n -= 4;
    }
    while (n > 0) {
        *p++ = v;
        n--;
    }
}
