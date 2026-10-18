#include "global.h"
#include "scene_vec.h"
#define SpriteNotice_CreateBetweenPositions sub_0812549C
#define SpriteNotice_Create sub_08122FC4
s32 Div(s32,s32);
s32 SpriteNotice_Create(struct SceneVec *,u32,u32);
s32 sub_0812549C(struct SceneVec *start,struct SceneVec *end,u32 arg)
{
    struct SceneVec values;
    struct SceneVec *delta=&values;
    values.x=end->x-start->x;
    delta->y=end->y-start->y;
    delta->z=end->z-start->z;
    if (values.x*values.x+delta->z*delta->z>0x3FF) {
        values.x=start->x+Div(values.x,2);
        delta->y=start->y;
        delta->z=start->z+Div(delta->z,2);
        if (SpriteNotice_Create(delta,2,arg)<0) goto fail;
    }
    return 1;
fail:
    return 0;
}
