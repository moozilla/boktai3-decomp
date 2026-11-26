#include "monsters/axe_armor/axe_armor.h"
#include "engine/engine.h"
void AxeArmor_Update(void *);
void AxeArmor_Destroy(void *);
s32 AxeArmor_InitializeFromScript(void *, void *);
void *AxeArmor_Create(void *p)
{
    struct AxeArmor *r = Task_Create(8, sizeof(*r));
    if (r) {
        Task_SetCallbacks(r, AxeArmor_Update, AxeArmor_Destroy);
        if (AxeArmor_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
