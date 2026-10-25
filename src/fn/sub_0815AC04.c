#include "global.h"
#include "player.h"

struct ScriptActorPackedPoint {
    u32 x : 16, y : 16, z : 16, pad : 16;
};

s32 Script_GetActorIndex(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 Math_Atan2(s32, s32);

#ifdef NONMATCHING
// fuzzy: 88.1%
// scope shape 10 (differences once low registers are set aside)
s32 ScriptActor_TestFacingPointFromScript(void)
{
    struct ScriptActorPackedPoint point;
    struct Player *actor, *player;
    s32 index = Script_GetActorIndex();
    actor = gUnk_02000580[index];
    player = actor;
    if (actor != NULL && actor->tetherMode == 12 && Script_SeekToKeyword('p')) {
        s32 dx, dz, direction, angle;
        point.x = Script_GetValue();
        point.y = Script_GetValue();
        point.z = Script_GetValue();
        dx = (s16)point.x - actor->position.x;
        dz = (s16)point.z - actor->position.z;
        if (dx * dx + dz * dz <= 1400 * 1400) {
            direction = ((player->facing + 5) & 7) * 32;
            angle = Math_Atan2(dx, dz);
            direction -= 256;
            if ((u32)((angle - direction) & 255) - 33 > 190 && Script_SeekToKeyword('d') &&
                (u32)((Script_GetValue() - direction) & 255) - 33 > 190)
                return 1;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0815AC04);
#endif
