#include "global.h"
#include "engine.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 sub_0815C9EC(void);
s32 sub_0815CA08(void);
void PlayerCue_MoveTo(u8 *actor, struct PackedVec *point, s32 direction, s32 f, s32 e);
void sub_0814F828(u8 *);
/* Script: PlayerCue_MoveTo on the scripted actor chosen by -i, with the optional
   point -p x y z, -d (default -1), -f (default 0) and -e (default 0);
   tentative: turn to a direction or point. */
void ScriptActor_TurnFromScript(void)
{
    struct PackedVec point;
    u8 *actor;
    s32 i = sub_0815A400();
    actor = gUnk_02000580[i];
    if (actor) {
        s32 f, direction, e;
        if (Script_SeekToKeyword('f'))
            f = Script_GetValue();
        else
            f = 0;
        direction = sub_0815C9EC();
        e = sub_0815CA08();
        if (Script_SeekToKeyword('p')) {
            point.x = Script_GetValue();
            point.y = Script_GetValue();
            point.z = Script_GetValue();
            PlayerCue_MoveTo(actor, &point, direction, f, e);
        } else
            PlayerCue_MoveTo(actor, 0, direction, f, e);
        sub_0814F828(actor);
    }
}
