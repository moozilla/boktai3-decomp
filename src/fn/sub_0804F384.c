#include "global.h"
struct M {
    u32 a;
    u16 *b;
};
s32 MessageQueue_GetMessagesById(u32, struct M **);
void ResourceStation_ReadDisplayCommands(u8 *p)
{
    struct M *m;
    s32 n = MessageQueue_GetMessagesById(*(u16 *)(p + 0x260), &m);
    s32 i = n;
    i--;
    if (n > 0) {
        u8 *t = p + 0x265;
        struct M *e = m;
        do {
            switch (*e->b) {
                case 0:
                    *t = 1u;
                    break;
                case 1:
                    *t = 0u;
                    break;
            }
            e++;
        } while (i-- > 0);
        m = e;
    }
}
