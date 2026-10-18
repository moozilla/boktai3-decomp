#define SceneActor_RunPositionScript sub_080B0C80
/* Descriptive aliases and object roles are tentative (confidence: low). */
#include "global.h"
#include "directional_interaction.h"
struct SceneActorPositionScript {
    u8 unknown0[0x5c];
    s16 x, y, z;
    u8 unknown62[0x67c - 0x62];
    u16 field67C;
    s16 argument, field680;
    u16 field682;
    u32 script;
    u8 unknown688[0x6d4 - 0x688];
    u16 flags;
};
void Sound_PlaySong(u32);
void sub_080B0C80(struct SceneActorPositionScript *p)
{
    s32 values[4];
    struct DirectionalScriptArgs args;
    if (p->script) {
        values[0] = p->x;
        values[1] = p->y;
        values[2] = p->z;
        values[3] = p->argument;
        args.count = 4;
        args.values = values;
        Script_Call(p->script, &args);
        p->field67C = 0xffff;
        p->argument = -1;
        p->field680 = -1;
        p->field682 = 0;
        {
            u16 *flags = &p->flags;
            s32 mask = -2;
            *flags = mask & *flags;
        }
        Sound_PlaySong(0xd6);
    }
}
