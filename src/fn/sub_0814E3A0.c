#define Player_UpdateAttachedEntryPosition sub_0814E3A0
#define Player_GetRaisedPosition sub_08159284
#define SceneEntry_SetPosition sub_081DAEE8
#include "global.h"
#include "player.h"
#include "scene_vec.h"
void sub_081DAD94(s32, s32);
void sub_08159284(s32, struct SceneVec *);
void sub_081DAEE8(s32, struct SceneVec *);
void sub_0814E3A0(struct Player *p)
{
    struct SceneVec position;
    s32 *entry = &p->statModifierContext.attachedEntry;
    if (*entry >= 0) {
        sub_081DAD94(p->unk2D, *entry);
        sub_08159284(0, &position);
        sub_081DAEE8(*entry, &position);
    }
}
