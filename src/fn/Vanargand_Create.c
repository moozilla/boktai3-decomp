#include "global.h"
#include "engine.h"
#include "ram.h"
void *SceneActors_GetBySlot(u32);
void Vanargand_Update(void *);
void SceneComposite_Destroy(void *);
s32 sub_081AE0C0(void *, void *);
/* Registry routine 7C56: creates the Vanargand task once; -t (default 5) is
   passed to SceneActors_GetBySlot, which can supply an existing one. */
void *Vanargand_Create(void *arg)
{
    void *p;
    if (gSceneComposite)
        return gSceneComposite;
    p = SceneActors_GetBySlot(Script_GetKeywordValue('t', 5));
    if (p)
        return p;
    p = Task_Create(9, 0xf48);
    if (p) {
        Task_SetCallbacks(p, Vanargand_Update, SceneComposite_Destroy);
        if (sub_081AE0C0(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gSceneComposite = p;
    return p;
}
