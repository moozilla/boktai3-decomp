s32 Trigger_RunScript(struct Trigger *p, u32 scriptInput, s32 event)
{
    u16 script;
    s32 values[10];
    struct TriggerScriptArgs args;
    s32 actor, i;
    s32 savedActor;
    u32 *source;
    s32 *out;
    struct TriggerScriptArgs *argsPtr;
    script = scriptInput;
    if (script) {
        actor = p->actor;
        if (actor != -1) {
            s32 index = actor;
            u32 mask = 12;
            u32 flags = gUnk_02000560[index] & mask;
            savedActor = index;
            if (!flags)
                goto active;
        }
    }
    return -1;
active:
    if (p->type == 1) {
        values[0] = p->position->x;
        values[1] = p->position->y + 64;
        values[2] = p->position->z;
    } else {
        values[0] = p->position->x;
        values[1] = p->position->y;
        values[2] = p->position->z;
    }
    values[3] = p->id;
    values[4] = p->kind;
    argsPtr = &args;
    source = p->arguments;
    out = values + 5;
    for (i = 0; i < 3; i++)
        *out++ = *source++;
    values[8] = savedActor;
    values[9] = event;
    {
        u32 mask = 0xffff0000;
        {
            u32 word = *(u32 *)argsPtr & mask;
            word |= 10;
            *(u32 *)argsPtr = word;
        }
    }
    argsPtr->values = values;
    return Script_Call(script, argsPtr);
}
