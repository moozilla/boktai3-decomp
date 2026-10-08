#include "global.h"

void sub_080287DC(void);
void sub_08024CBC(void *);
void sub_08028394(void *);
void sub_080289FC(void *);

s32 sub_08028F24(void *p)
{
    sub_080287DC();
    sub_08024CBC(p);
    sub_08028394(p);
    sub_080289FC(p);
    return 0;
}
