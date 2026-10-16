#include "global.h"
#include "engine.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 sub_0815C9EC(void);
s32 sub_0815CA08(void);
void sub_0815BF18(u8 *actor, struct PackedVec *point, s32 direction, s32 e);
void sub_0814F828(u8 *);
/* Sibling of ScriptActor_MoveToFromScript (not referenced directly by the
   registry): -p x y z, -d and -e for sub_0815BF18. */
void sub_0815CC78(void)
{
    struct PackedVec point;
    u8 *actor;
    s32 i = sub_0815A400();
    actor = gUnk_02000580[i];
    if (actor && Script_SeekToKeyword('p')) {
        s32 direction, e;
        point.x = Script_GetValue();
        point.y = Script_GetValue();
        point.z = Script_GetValue();
        direction = sub_0815C9EC();
        e = sub_0815CA08();
        sub_0815BF18(actor, &point, direction, e);
        sub_0814F828(actor);
    }
}
