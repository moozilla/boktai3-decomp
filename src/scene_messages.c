#include "scene_messages.h"

struct SceneMessageReceiver *SceneMessages_FindReceiver(struct SceneMessages *controller, u32 id,
                                                        u32 kind)
{
    struct SceneMessageReceiver *receiver = controller->receivers;
    while (receiver) {
        struct SceneMessageReceiver *next = receiver->next;
        if (receiver->queue.id == id && receiver->queue.kind == kind)
            return receiver;
        receiver = next;
    }
    return 0;
}

u32 SceneMessages_LinkReceiver(struct SceneMessages *controller,
                               struct SceneMessageReceiver *receiver)
{
    if (controller->receivers)
        controller->receivers->prev = receiver;
    receiver->prev = 0;
    receiver->next = controller->receivers;
    controller->receivers = receiver;
    return 0;
}

u32 SceneMessages_UnlinkReceiver(struct SceneMessages *controller,
                                 struct SceneMessageReceiver *receiver)
{
    struct SceneMessageReceiver *adjacent = receiver->prev;
    if (adjacent)
        adjacent->next = receiver->next;
    else
        controller->receivers = receiver->next;
    adjacent = receiver->next;
    if (adjacent)
        adjacent->prev = receiver->prev;
    return 0;
}

int SceneMessages_Enqueue(struct SceneMessages *controller, struct SceneEventCommand *message)
{
    struct SceneMessageReceiver *entry =
        SceneMessages_FindReceiver(controller, message->id, message->receiverKind);
    int buffer = 1 - controller->buffer;
    if (entry && entry->queue.count[buffer] < 4) {
        entry->queue.pending[buffer][entry->queue.count[buffer]] = message;
        entry->queue.count[buffer]++;
    }
}

void SceneMessages_SwapBuffers(struct SceneMessages *controller)
{
    struct SceneMessageReceiver *entry = controller->receivers;
    while (entry) {
        struct SceneMessageReceiver *next = entry->next;
        int index;
        entry->queue.count[controller->buffer] = 0;
        for (index = 0; index < 4; index++)
            entry->queue.pending[controller->buffer][index] = 0;
        entry = next;
    }
    controller->buffer = 1 - controller->buffer;
}

void SceneMessages_RunScript(struct SceneMessages *controller, struct SceneEventReceiver *entry,
                             struct SceneEventCommand *message)
{
    int args[8];
    struct SceneMessageArguments call;
    int id = (u16)SceneMessages_FindScript((u16)message->value);
    if (id) {
        int count = message->argumentCount;
        if (count > 1) {
            s16 *in = &message->value + 1;
            int *out = args;
            int remaining = count - 1;
            do {
                do {
                    *(out++) = *(in++);
                } while (0);
                remaining--;
            } while (remaining);
        }
        {
            u32 mask = 0xffff0000;
            u32 header;
            header = call.header;
            header &= mask;
            header |= 8;
            call.header = header;
        }
        call.values = args;
        Script_Call(id, &call);
    }
    Trigger_ConsumePending(entry, 1);
}

void SceneMessages_SetDelay(struct SceneMessages *controller, struct SceneEventReceiver *receiver,
                            struct SceneEventCommand *message)
{
    s32 delay = message->value;
    controller->delay = delay;
    if (delay == 0)
        Trigger_ConsumePending(receiver, 1);
}

void SceneMessages_SetSignal(struct SceneMessages *controller, struct SceneEventReceiver *receiver,
                             struct SceneEventCommand *message)
{
    controller->signal = 1;
    Trigger_ConsumePending(receiver, 1);
}

int SceneMessages_Dispatch(struct SceneMessages *controller)
{
    struct SceneEventReceiver *entry = &controller->self.queue;
    int index;
    index = 0;
    if (index < entry->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventCommand *message;
            struct SceneEventDispatchState *dispatchState = gSceneEventDispatchState;
            message = entry->pending[dispatchState->phase][index];
            SceneMessages_SetPendingTrigger(entry, message);
            switch (message->kind) {
                case 0:
                    SceneMessages_RunScript(controller, entry, message);
                    break;
                case 1:
                    SceneMessages_SetDelay(controller, entry, message);
                    break;
                case 2:
                    SceneMessages_SetSignal(controller, entry, message);
                    break;
            }
            index++;
        } while (index < entry->count[gSceneEventDispatchState->phase]);
    }
    return 0;
}

int SceneMessages_Update(struct SceneMessages *controller)
{
    short zero16;
    int dispatch = 0;
    int active = controller->active;
    if (active == 1) {
        controller->changed = 0;
        if (controller->advance) {
            {
                int next = ((u16)controller->step) + 1;
                int zero = 0;
                zero16 = zero;
                controller->step = next;
                controller->queued = 0;
                controller->advance = zero16;
                controller->changed = active;
            }
        }
        if (controller->changed) {
            struct SceneEventCommand *message;
            controller->queued = 0;
            while (message = SceneMessages_GetEventMessage(controller->scene, controller->step,
                                                           controller->queued)) {
                SceneMessages_Enqueue(controller, message);
                (controller->queued)++;
            }

            if (!(controller->queued)) {
                if (controller->onFinish) {
                    Script_Call(controller->onFinish, 0);
                }
                controller->active = 0;
                return 0;
            }
            dispatch = 1;
        }
        if (controller->delay) {
            if ((--(controller->delay)) == 0) {
                Trigger_ConsumePending(&controller->self.queue, 1);
            }
        }
    }
    SceneMessages_SwapBuffers(controller);
    if (dispatch) {
        SceneMessages_Dispatch(controller);
    }
    return 0;
}

u32 SceneMessages_Destroy(struct SceneMessages *controller)
{
    SceneMessages_UnregisterReceiver(&controller->self);
    gSceneEventDispatchState = 0;
    return 0;
}

int SceneMessages_Initialize(struct SceneMessages *controller, int id)
{
    gSceneEventDispatchState = (struct SceneEventDispatchState *)controller;
    controller->active = 0;
    controller->scene = 0xffff;
    controller->step = -1;
    controller->queued = 0;
    controller->advance = 0;
    controller->changed = 0;
    controller->onFinish = 0;
    controller->scriptCount = 0;
    controller->unknown6C = 0;
    controller->receivers = 0;
    SceneMessages_RegisterReceiver(&controller->self, id, 1);
    return 0;
}

struct SceneMessages *SceneMessages_Create(int id)
{
    struct SceneMessages *controller;
    if (gSceneEventDispatchState)
        return 0;
    controller = Task_Create(2, sizeof(struct SceneMessages));
    if (controller) {
        Task_SetCallbacks(controller, (void (*)(void *))SceneMessages_Update,
                          (void (*)(void *))SceneMessages_Destroy);
        if (SceneMessages_Initialize(controller, id) < 0) {
            Task_Kill(controller);
            return 0;
        }
    }
    return controller;
}

int SceneMessages_RegisterReceiver(struct SceneMessageReceiver *receiver, int id, int kind)
{
    int result;
    if (gSceneEventDispatchState) {
        int zero = 0;
        receiver->queue.id = id;
        receiver->queue.kind = kind;
        receiver->queue.count[0] = zero;
        receiver->queue.count[1] = zero;
        receiver->queue.trigger = zero;
        result =
            SceneMessages_LinkReceiver((struct SceneMessages *)gSceneEventDispatchState, receiver);
    } else
        result = -1;
    return result;
}

s32 SceneMessages_UnregisterReceiver(struct SceneMessageReceiver *receiver)
{
    s32 result;
    if (gSceneEventDispatchState)
        result = SceneMessages_UnlinkReceiver((struct SceneMessages *)gSceneEventDispatchState,
                                              receiver);
    else
        result = -1;
    return result;
}

s32 SceneMessages_RequestAdvance(void)
{
    struct SceneMessages *controller = (struct SceneMessages *)gSceneEventDispatchState;
    if (controller == 0)
        return -1;
    if (controller->advance != 0)
        return -2;
    controller->advance = 1;
    return 0;
}

u32 SceneMessages_SetPendingTrigger(struct SceneEventReceiver *receiver,
                                    struct SceneEventCommand *message)
{
    u8 trigger = message->trigger;
    u32 result;
    if (trigger == 0) {
        result = 0;
    } else {
        receiver->trigger = trigger;
        result = 1;
    }
    return result;
}

u32 Trigger_ConsumePending(void *latch, u32 trigger)
{
    struct SceneEventReceiver *receiver = latch;
    u32 result;
    if (receiver->trigger != trigger)
        result = 0;
    else {
        SceneMessages_RequestAdvance();
        receiver->trigger = 0;
        result = 1;
    }
    return result;
}

int SceneMessages_FindScript(int id)
{
    struct SceneMessages *controller = (struct SceneMessages *)gSceneEventDispatchState;
    int index;
    if (controller)
        goto scan;
    goto missing;
found:
    return controller->scripts[index];
scan:
    for (index = 0; index < controller->scriptCount; index++)
        if (controller->ids[index] == id)
            goto found;
missing:
    return 0;
}

s32 SceneMessages_Send(struct SceneEventCommand *message)
{
    s32 result;
    if (gSceneEventDispatchState)
        result = SceneMessages_Enqueue((struct SceneMessages *)gSceneEventDispatchState, message);
    else
        result = -1;
    return result;
}

int SceneMessages_ScriptStart(void)
{
    struct SceneMessages *controller = (struct SceneMessages *)gSceneEventDispatchState;
    struct SceneMessageReceiver *self;
    u8 *scriptCount;
    int idCount;
    int procedureCount;
    int index;
    int hasScriptIds;
    if (!controller || controller->active == 1)
        return -1;
    controller->active = 1;
    controller->scene = Script_GetKeywordValue('d', -1);
    controller->step = Script_GetKeywordValue('c', -1) - 1;
    controller->queued = 0;
    controller->advance = 1;
    controller->changed = 0;
    controller->onFinish = Script_GetKeywordValue('e', 0);
    controller->scriptCount = 0;
    idCount = 0;
    procedureCount = 0;
    hasScriptIds = Script_SeekToKeyword('p');
    scriptCount = &controller->scriptCount;
    self = &controller->self;
    if (hasScriptIds) {
        for (index = 0; index < 16 && Script_GetPc(); index++) {
            controller->ids[idCount] = Script_GetValue();
            idCount++;
        }
    }
    if (Script_SeekToKeyword('P')) {
        for (index = 0; index < 16 && Script_GetPc(); index++) {
            controller->scripts[procedureCount] = Script_GetValue();
            procedureCount++;
        }
    }
    if (idCount > 0 || procedureCount > 0)
        *scriptCount = idCount;
    controller->unknown6C = 0;
    self->queue.count[0] = 0;
    self->queue.count[1] = 0;
    self->queue.trigger = 0;
    return 0;
}

int SceneMessages_Stop(void)
{
    struct SceneMessages *controller = (struct SceneMessages *)gSceneEventDispatchState;
    int result;
    if (controller) {
        controller->active = 0;
        controller->scene = 0xffff;
        controller->step = -1;
        controller->queued = 0;
        controller->advance = 0;
        controller->changed = 0;
        controller->onFinish = 0;
        controller->scriptCount = 0;
        controller->unknown6C = 0;
        result = 0;
    } else
        result = -1;
    return result;
}

int SceneMessages_ConsumeSignal(void)
{
    struct SceneMessages *controller = (struct SceneMessages *)gSceneEventDispatchState;
    if (!controller || !controller->signal)
        return -1;
    controller->signal = 0;
    SceneMessages_RequestAdvance();
    return 0;
}

int SceneMessages_IsActive(void)
{
    struct SceneMessages *controller = (struct SceneMessages *)gSceneEventDispatchState;
    int result;
    if (!controller || controller->active != 1)
        result = 0;
    else
        result = 1;
    return result;
}
