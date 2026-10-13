#include "global.h"
struct Escape {
    u16 key, value;
};
extern const struct Escape gUnk_086055E4[];
u16 sub_08032070(const u8 *p)
{
    const struct Escape *t = gUnk_086055E4;
    int key = t->key;
    if (key) {
        int c = p[1];
        do {
            if (key == c)
                return t->value;
            t++;
            key = t->key;
        } while (key);
    }
    return 0xFFFF;
}
