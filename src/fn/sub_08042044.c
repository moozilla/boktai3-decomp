#include "global.h"

struct S {
    u8 f[0xe34];
    u8 a;
};
void ModifierMenu_DestroyDisplayObjects(struct S *);
void CharacterModel_RemoveAnimations(u8 *);
void ModifierMenu_ReleaseTextResources(struct S *);
u32 ModifierMenu_DestroyTask(struct S *p)
{
    ModifierMenu_DestroyDisplayObjects(p);
    CharacterModel_RemoveAnimations(&p->a);
    ModifierMenu_ReleaseTextResources(p);
    return 0;
}
