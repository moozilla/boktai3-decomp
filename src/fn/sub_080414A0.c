#include "global.h"

void sub_080335B4(void);
void sub_08041458(u8 *);

void sub_080414A0(u8 *p) {
    u8 *f = p + 0x116;
    if (*f == 0) {
        sub_080335B4();
        sub_08041458(p);
        *f = 1;
    }
}
