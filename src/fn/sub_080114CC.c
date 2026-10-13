#include "message_menu.h"
void MessageMenu_UpdateTask(void);
void MessageMenu_DestroyTask(void);
void *sub_080114CC(void)
{
    void *obj;
    obj = Task_Create(11, 0x4D8);
    if (obj != 0) {
        Task_SetCallbacks(obj, MessageMenu_Update, MessageMenu_Destroy);
        if (MessageMenu_Initialize(obj) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
