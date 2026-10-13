#include "global.h"
#include "scene_object.h"
// MATCHING_CONCESSION: register-pinning
s32 sub_0821ABA8(s32, s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
#define RESOURCE(value)                                                                            \
    ({                                                                                             \
        s32 result = (value);                                                                      \
        asm("" : "+r"(result));                                                                    \
        result;                                                                                    \
    })
s32 SceneObjects_CreateFromScript(void)
{
    struct SceneExtent position;
    s32 id, type;
    s32 initial, animation, script, defaultResource;
    register s32 flags asm("r9"), resource asm("r6"), drawMode asm("r8"), health asm("r10");
    id = sub_0821ABA8('n', 0);
    type = sub_0821ABA8('t', 0);
    flags = sub_0821ABA8('f', 0);
    if (Script_SeekToKeyword('M') && Script_GetValue())
        flags |= 16;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    if (type == 0)
        goto fallback;
    else if (type == 1)
        defaultResource = RESOURCE(0x84EE);
    else if (type == 2)
        defaultResource = RESOURCE(0x9D40);
    else if (type == 3)
        defaultResource = RESOURCE(0x9D43);
    else if (type == 4)
        defaultResource = RESOURCE(0xE0C9);
    else if (type == 5)
        defaultResource = RESOURCE(0x5A2);
    else if (type == 6)
        defaultResource = RESOURCE(0xB306);
    else {
    fallback:
        defaultResource = RESOURCE(0x9D41);
    }
    resource = sub_0821ABA8('a', defaultResource);
    drawMode = sub_0821ABA8('i', 0);
    health = sub_0821ABA8('l', 128);
    if (type == 0)
        initial = 0x2B8;
    else if (type == 1)
        initial = 0x2C2;
    else if (type == 2)
        initial = 0x279;
    else if (type == 3)
        initial = 0x6E;
    else if (type == 4)
        initial = 0x1D0;
    else if (type == 5)
        initial = 0xC4;
    else if (type == 6) {
        if (drawMode == 0 || drawMode == 2)
            initial = 4;
        else
            initial = 5;
    } else
        initial = sub_0821ABA8('d', -1);
    animation = 0x192;
    script = sub_0821ABA8('b', 0);
    SceneObjects_CreateObject(id, type, flags, (struct SceneVec *)&position, resource, drawMode, health, initial,
                              animation, script);
    return 0;
}
