#include "repeated_effect.h"
#include "contact_actor.h"
void RepeatedEffect_AddFromScript(void)
{
    struct RepeatedEffectManager *manager = gRepeatedEffectManager;
    s32 tag, initialState, bodyEnabled, behaviorFlags, constrainCell;
    struct ContactPackedVector position;
    if (!manager)
        manager = RepeatedEffect_CreateManager();
    if (manager->count > 7)
        return;
    if (Script_SeekToKeyword('n'))
        tag = Script_GetValue();
    else
        tag = 0;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    if (Script_SeekToKeyword('s'))
        initialState = Script_GetValue();
    else
        initialState = 0;
    if (Script_SeekToKeyword('h'))
        bodyEnabled = Script_GetValue();
    else
        bodyEnabled = 0;
    if (Script_SeekToKeyword('f'))
        behaviorFlags = Script_GetValue();
    else
        behaviorFlags = 0;
    if (Script_SeekToKeyword('m'))
        constrainCell = Script_GetValue();
    else
        constrainCell = 0;
    RepeatedEffect_InitEntry(manager, tag, (struct RepeatedEffectPosition *)&position, initialState,
                             bodyEnabled, behaviorFlags, constrainCell);
    manager->count++;
}
