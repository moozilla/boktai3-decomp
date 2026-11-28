#include "scene/motion/scene_actor_terrain.h"
static inline u16 ReadParameter(s32 keyword)
{
    return Script_GetKeywordValue(keyword, 0);
}
void SceneActorTerrain_ReadScriptParameters(struct SceneActorTerrainView *actor)
{
    actor->scriptParameterI = ReadParameter('I');
    if (Script_SeekToKeyword('G')) {
        u32 *parameters = actor->scriptParametersG;
        s32 i;
        for (i = 0; i < 5; i++) {
            u32 value;
            if (Script_GetPc())
                value = Script_GetValue();
            else
                value = 0;
            *parameters++ = value;
        }
    }
    actor->scriptParameterD = ReadParameter('D');
}
