#include "npc/story_character/scene_character.h"

void Actor_UpdateVisibilityAndShadow(void *, struct SceneNpc *);
void SceneCharacter_UpdatePaletteFade(void *, struct SceneNpc *);
void sub_081CDEF8(void *, struct SceneNpc *);

s32 SceneCharacter_UpdateDisplay(void *context, struct SceneNpc *actor)
{
    Actor_UpdateVisibilityAndShadow(context, actor);
    if (actor->kind == 29 || actor->kind == 9) {
        SceneCharacter_UpdatePaletteFade(context, actor);
        sub_081CDEF8(context, actor);
    }
    return 0;
}
