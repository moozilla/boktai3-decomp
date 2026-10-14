#include "global.h"

struct Block0821AF04 {
    u32 w[4];
};
extern struct Block0821AF04 gUnk_02000438, gUnk_02000448;

void sub_0821AF04(struct Block0821AF04 *src)
{
    gUnk_02000438 = src[0];
    gUnk_02000448 = src[1];
}
