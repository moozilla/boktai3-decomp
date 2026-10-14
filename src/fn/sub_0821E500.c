#include "global.h"
#include "engine.h"

u8 *sub_0821E104(u16, void *);

void sub_0821E500(u16 id, u16 *out)
{
    u32 tmp;
    u8 *e = sub_0821E104(id, &tmp);
    if (e) {
        out[0] = *(u16 *)(e + 4);
        out[1] = e[9] << 4;
        out[2] = *(u16 *)(e + 6);
    }
}
