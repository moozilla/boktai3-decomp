#include "global.h"
#include "projected_display.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Registry1909 {
    u8 task[0x18];
    struct ProjectedDisplayEntry animation;
    struct ProjectedDisplayResource resource;
    u8 receiver[0x38];
    u32 id;
};
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
void SceneMessages_RegisterReceiver(void *, int, int);
void Registry1909_EnterHiddenState(struct Registry1909 *);
static inline void ClearVector(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}
s32 sub_081C5308(struct Registry1909 *p, u32 id)
{
    struct ProjectedDisplayResource *resource = ScriptProc_Start(0xCB05, 0x35E3);
    struct ProjectedDisplayResource *header, *savedHeader;
    u8 zero[4];
    struct PackedVec position;
    struct PackedVec *pos;
    if (!resource)
        return -1;
    p->resource = *resource;
    header = &p->resource;
    ResourceHeader_Relocate(header, resource);
    {
        u32 found = Script_SeekToKeyword('p');
        savedHeader = header;
        if (found) {
            position.x = Script_GetValue();
            position.y = Script_GetValue();
            position.z = Script_GetValue();
            pos = &position;
        } else {
            ClearVector(&position, (u32 *)zero);
            pos = &position;
        }
    }
    Widget_InitLinked(&p->animation, savedHeader, 0, 0, 2, 0, 60, pos);
    Anim_Step(&p->animation, savedHeader);
    p->id = id;
    SceneMessages_RegisterReceiver(p->receiver, id, 10);
    Registry1909_EnterHiddenState(p);
}
