#include "global.h"
#include "engine.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 sub_0815C9EC(void);
s32 sub_0815CA08(void);
void PlayerCue_WalkTo(u8 *actor, struct PackedVec *destination, s32 e, s32 direction);
void sub_0814F828(u8 *);
/* Script: -p x y z destination for the scripted actor chosen by -i, with -d
   (default -1) and -e (default 0); tentative: starts a move to that point. */
void ScriptActor_MoveToFromScript(void)
{
    struct PackedVec destination;
    u8 *actor;
    s32 i = sub_0815A400();
    actor = gUnk_02000580[i];
    if (actor && Script_SeekToKeyword('p')) {
        s32 direction, e;
        destination.x = Script_GetValue();
        destination.y = Script_GetValue();
        destination.z = Script_GetValue();
        direction = sub_0815C9EC();
        e = sub_0815CA08();
        PlayerCue_WalkTo(actor, &destination, e, direction);
        sub_0814F828(actor);
    }
}
