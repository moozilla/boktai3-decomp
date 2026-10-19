#include "global.h"

void SceneActor_DispatchCommands(u8 *);

void RegistryA8CB_ResetUpdateState(u8 *obj)
{
    *(u16 *)(obj + 0x8BA) = 0;
    SceneActor_DispatchCommands(obj);
}
