#include "global.h"
struct SceneEffectCommand {
    s16 x, y, kind;
};
void sub_08115AC8(s32, s32, s32, s32);
void sub_081160CC(s32, s32, s32);
void SceneEffect_ExecuteCommandList(const s16 *commands, s32 length)
{
    s32 i;
    for (i = 0; i < length; i += 3) {
        switch (commands[i + 2]) {
            case 0:
                sub_08115AC8(commands[i], commands[i + 1], 0, 30);
                break;
            case 4:
                sub_08115AC8(commands[i], commands[i + 1], 2, 60);
                break;
            case 1:
                sub_08115AC8(commands[i], commands[i + 1], 1, 80);
                break;
            case 2:
                sub_081160CC(commands[i], commands[i + 1], 0);
                break;
            case 3:
                sub_081160CC(commands[i], commands[i + 1], 1);
                break;
        }
    }
}
