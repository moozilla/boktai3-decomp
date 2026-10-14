#include "global.h"

struct Block0821AEDC {
    u32 w[4];
};
extern struct Block0821AEDC gUnk_02000438, gUnk_02000448;

void sub_0821AEDC(struct Block0821AEDC *dst)
{
    dst[0] = gUnk_02000438;
    dst[1] = gUnk_02000448;
}
