#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad[48];
    struct Vec position;
};
struct Controller {
    u16 id;
    u8 orientation, flags, disabled, state, entered, level;
    s32 region;
    u16 startScript, endScript;
    u32 distance;
    s16 arguments[4];
    struct Vec position;
    u32 drawableFlags;
    u8 rest[68];
};
struct Manager {
    u8 pad[24];
    u32 count : 8;
    s32 selected : 8;
    s32 pending : 8;
    u32 unused : 8;
    u32 enabled;
    struct Controller *slots;
    struct Actor *actor;
    struct Vec position;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern struct Manager *gUnk_02000038;
s32 sub_0821ABA8(u32, s32);
struct Controller *sub_08009568(struct Manager *, s32, u32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void sub_080095D4(struct Manager *, struct Controller *, struct PackedVec *);
s32 sub_0822C034(struct Vec *);
s32 DirectionalController_ScriptMove(void)
{
    struct PackedVec position;
    struct Manager *manager = gUnk_02000038;
    struct Controller *p;
    if (!manager)
        goto fail;
    p = sub_08009568(manager, sub_0821ABA8('n', 0), 0);
    if (!p) {
    fail:
        return -1;
    }
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    sub_080095D4(manager, p, &position);
    if (p->region >= 0)
        p->region = sub_0822C034(&p->position);
}
