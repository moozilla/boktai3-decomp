#include "global.h"
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int sub_08013D3C(int, int, void *);
#define ShadowRegistry_CreateFromScript sub_08013DC0
int ShadowRegistry_CreateFromScript(void)
{
    struct Packed pos;
    int result;
    int id = sub_0821ABA8('n', 0);
    int mode = sub_0821ABA8('m', 1);
    if (!Script_SeekToKeyword('p')) {
        result = -1;
    } else {
        pos.x = Script_GetValue();
        pos.y = Script_GetValue();
        pos.z = Script_GetValue();
        result = sub_08013D3C(id, mode, &pos);
    }
    return result;
}
