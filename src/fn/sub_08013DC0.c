#include "global.h"
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int ShadowRegistry_Create(int, int, void *);
int ShadowRegistry_CreateFromScript(void)
{
    struct Packed pos;
    int result;
    int id = Script_GetKeywordValue('n', 0);
    int mode = Script_GetKeywordValue('m', 1);
    if (!Script_SeekToKeyword('p')) {
        result = -1;
    } else {
        pos.x = Script_GetValue();
        pos.y = Script_GetValue();
        pos.z = Script_GetValue();
        result = ShadowRegistry_Create(id, mode, &pos);
    }
    return result;
}
