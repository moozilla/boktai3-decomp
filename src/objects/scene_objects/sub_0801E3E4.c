#include "global.h"
typedef struct {
    s16 x, y, z, pad;
} Vec;
void *ResourceRegistry_GetResource(int);
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void Terrain_GetBoxMinPosition(u16, void *);
int SceneObjects_Append(void *, u16, void *, int, u8, u8, u8, int, int, int, int, void *, u8, int);
int SceneObjects_ScriptAppend(void)
{
    int args[4];
    Vec position;
    void *p;
    u16 id;
    int value;
    u8 kind, variant, orientation;
    int type, b45, maxLift;
    u8 b44;
    int hasArgs, b41, i, found;
    p = ResourceRegistry_GetResource(0x85e6);
    if (!p)
        return -1;
    id = Script_GetKeywordValue(0x6e, 0);
    value = Script_GetKeywordValue(0x64, 0);
    kind = Script_GetKeywordValue(0x66, 0);
    variant = Script_GetKeywordValue(0x73, 0);
    orientation = Script_GetKeywordValue(0x72, 0);
    type = Script_GetKeywordValue(0x76, 0);
    b45 = Script_GetKeywordValue(0x63, 0);
    maxLift = Script_GetKeywordValue(0x68, 400);
    b44 = Script_GetKeywordValue(0x6d, 0);
    if (Script_SeekToKeyword(0x6b)) {
        hasArgs = Script_GetValue();
        b41 = Script_GetValue();
        for (i = 0; i < 4; i++)
            args[i] = Script_GetValue();
    } else {
        hasArgs = 0;
        b41 = 0;
    }
    if (Script_SeekToKeyword(0x74))
        Terrain_GetBoxMinPosition(Script_GetValue(), &position);
    else {
        found = Script_SeekToKeyword(0x70);
        if (found) {
            position.x = Script_GetValue();
            position.y = Script_GetValue();
            position.z = Script_GetValue();
        } else {
            position.x = found;
            position.y = found;
            position.z = found;
        }
    }
    return SceneObjects_Append(p, id, &position, value, kind, variant, orientation, type, maxLift,
                               hasArgs, b41, args, b44, b45);
}
