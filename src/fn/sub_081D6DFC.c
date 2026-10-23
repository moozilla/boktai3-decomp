#include "global.h"

struct ScaledByte {
    u8 unk0[0x48];
    u8 value;
    u8 unk49[0x8E - 0x49];
    u16 scaled;
};

void sub_081D6DFC(struct ScaledByte *p)
{
    p->scaled = p->value << 4;
}
