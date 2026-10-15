#include "scene_actor_script.h"
#define SceneActorScript_SetPositionFromScript sub_080722C0
static inline s32 ReadOrZero(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
static inline u32 CellAt(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline s32 GroundHeight(u32 cell, struct ContactPosition *position)
{
    u8 *override = sub_0821D64C(cell, 1);
    u8 *data;
    u32 type;
    s32 height;
    if (override)
        data = override + 4;
    else
        data = gUnk_030052F4->cells + 12 + cell * 4;
    type = data[0] >> 4;
    height = (data[0] & 15) << 8;
    {
        u32 slope;
        if (type == 1)
            goto alongZ;
        if (type == 2)
            goto alongX;
        goto finished;
    alongZ:
        slope = (u8)position->z;
        goto subtract;
    alongX:
        slope = (u8)position->x;
    subtract:
        height -= slope;
    }
finished:
    return height;
}
void SceneActorScript_SetPositionFromScript(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    struct SceneActorScriptPositionView *actor;
    struct ContactPosition *position;
    s16 *height;
    if (!id)
        return;
    actor = (void *)sub_08070EBC(id);
    if (!actor)
        return;
    if (!Script_SeekToKeyword('p'))
        return;
    actor->position.x = ReadOrZero();
    actor->position.y = ReadOrZero();
    {
        u8 *pc = Script_GetPc();
        s32 value;
        height = &actor->position.y;
        if (pc)
            value = Script_GetValue();
        else
            value = 0;
        actor->position.z = value;
    }
    position = &actor->position;
    actor->cellIndex = CellAt(position->x >> 8, position->z >> 8);
    actor->groundHeight = GroundHeight(actor->cellIndex, position);
    actor->heightOffset = *height - actor->groundHeight;
    *height = actor->groundHeight;
}
