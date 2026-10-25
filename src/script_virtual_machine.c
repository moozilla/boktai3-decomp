#include "script_virtual_machine.h"

s32 Script_FindResourceKey(u32 key, u16 *tbl, s32 x, s32 lo, s32 hi)
{
    s32 mid;
    while (lo < hi) {
        mid = (lo + hi) >> 1;
        if (tbl[mid] < key)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (tbl[lo] == key)
        return lo;
    return -1;
}

struct ScriptResourceHeader {
    u32 count;
    u32 keys, records, reserved;
};

u8 *Script_LookupResource(u8 *base, u32 unused, u16 key, u16 context)
{
    struct ScriptResourceHeader header = *(struct ScriptResourceHeader *)base;
    s32 index;
    header.keys += (u32)base;
    header.records += (u32)base;
    index = Script_FindResourceKey(key, (u16 *)header.keys, context, 0, header.count - 1);
    if (index < 0 || (u32)index >= header.count)
        return 0;
    return (u8 *)(((u32 *)header.records)[index] + (u32)base);
}

u32 Script_LookupArchive(u32 key, struct ScriptArchiveEntry *tbl, s32 lo, s32 hi)
{
    s32 mid;
    struct ScriptArchiveEntry *e;
    while (lo < hi) {
        mid = (lo + hi) >> 1;
        if (tbl[mid].key < key)
            lo = mid + 1;
        else
            hi = mid;
    }
    e = (struct ScriptArchiveEntry *)((lo << 3) + (u32)tbl);
    if (e->key == key)
        return e->value;
    return 0;
}

extern struct ScriptArchiveEntry gUnk_08614D6C[];

void *ScriptProc_Start(u16 key, u16 context)
{
    u32 special = 0;
    u16 saved_context = context;
    u32 combined;
    u8 *result;
    switch (key) {
        case 0x922E:
            key = 0x9225;
            context = 0x5130;
            special = 1;
            break;
        case 0x92B3:
            key = 0x9305;
            context = 0xD710;
            special = 1;
            break;
        case 0x9B1B:
            key = 0x9A65;
            context = 0x4679;
            special = 1;
            break;
        case 0x98F5:
            key = 0x9B05;
            context = 0x2117;
            special = 1;
            break;
        case 0xA635:
            key = 0xA705;
            context = 0x6D24;
            special = 1;
            break;
        case 0xAE6C:
            key = 0xAF05;
            context = 0xAC2C;
            special = 1;
            break;
        case 0xC091:
            key = 0xC305;
            context = 0xE53E;
            special = 1;
            break;
        case 0xCB05:
            key = 0xC8E5;
            context = 0x5F29;
            special = 1;
            break;
        case 0xCEEF:
            key = 0xCEE5;
            context = 0x4F2D;
            special = 1;
            break;
        case 0xCEAA:
            key = 0xCF05;
            context = 0xA4D;
            special = 1;
            break;
    }
    combined = (key << 16) | context;
    result = (u8 *)Script_LookupArchive(combined, gUnk_08614D6C, 0, 10);
    if (special == 1) {
        u32 third = saved_context;
        result = Script_LookupResource(result, key, third, combined);
    }
    return result;
}

u8 *Script_DecodePacket(u8 *p, u32 *length)
{
    s32 size = *p & 15;
    switch (size) {
        case 13:
            *length = p[1];
            return p + 2;
        case 14: {
            u32 high = p[2] << 8;
            *length = p[1] | high;
            return p + 3;
        }
        case 15: {
            u8 *q = p + 1;
            *length = (q[2] << 16) | (q[1] << 8) | p[1];
            return p + 4;
        }
        default:
            *length = size;
            return p + 1;
    }
}

u8 *Script_DecodeCommandOffset(u8 *p, u32 *out)
{
    if (p[0] & 0x80) {
        *out = ((p[0] << 8) | p[1]) & 0x7FFF;
        return p + 2;
    }
    *out = p[0];
    return p + 1;
}

u8 *Script_DecodeOperand(u8 *p, s32 *type, s32 *value)
{
    u32 length;
    u8 *cursor = p;
    s32 opcode = *cursor;
    s32 family = opcode & 0xF0;
    if ((family & 0xC0) == 0xC0) {
        *type = 9;
        *value = (*cursor & ~0xC0) - 1;
        cursor++;
    } else if (!family) {
        *type = opcode;
        cursor++;
        switch (opcode) {
            case 0:
                cursor = 0;
                break;
            case 8: {
                s32 high = cursor[1] << 8;
                *value = cursor[0] | high;
                cursor += 2;
                break;
            }
            case 1: {
                s32 high = cursor[1] << 8;
                *value = (s16)(cursor[0] | high);
                cursor += 2;
                break;
            }
            case 9:
            case 10:
            case 13:
                *value = (cursor[3] << 24) | (cursor[2] << 16) | (cursor[1] << 8) | cursor[0];
                cursor += 4;
                break;
            case 6: {
                s32 high = cursor[1] << 8;
                *value = high | cursor[0];
                cursor += 2;
                break;
            }
            case 2:
            case 3:
            case 4:
                *value = *cursor++;
                break;
            case 7:
                *value = (s32)(cursor + 1);
                cursor += *cursor + 1;
                break;
            case 14: {
                s32 high = cursor[1] << 8;
                *value = (s32)Text_LookupString(cursor[0] | high);
                *type = 7;
                cursor += 2;
                break;
            }
        }
    } else {
        *type = family;
        switch (family) {
            case 0x10:
            case 0x20:
                return Script_DecodeVariableValue(p, type, value);
            case 0x40:
                if ((*cursor & 15) == 15) {
                    *value = Script_GetCallArgument(cursor[1] + 15);
                    cursor++;
                } else
                    *value = Script_GetCallArgument(*cursor & 15);
                *type = 9;
                cursor++;
                break;
            case 0x90:
                *value = Script_GetStackArgument(*cursor & 15);
                *type = 9;
                cursor++;
                break;
            case 0x80:
                cursor = Script_DecodePacket(cursor, &length);
                *value = (s32)cursor;
                cursor += length;
                break;
            case 0x30:
                cursor = Script_DecodePacket(cursor, &length);
                *value = Script_EvalExpression(cursor);
                cursor += length;
                break;
            case 0x50:
                cursor = Script_DecodePacket(cursor, &length);
                *type |= *cursor << 16;
                *value = (s32)(cursor + 1);
                cursor += length;
                break;
        }
    }
    return cursor;
}

void Script_ResetArgumentStack(void)
{
    gUnk_02000610.argSp = gUnk_02000610.stack + 25;
}

u32 *Script_PushArgumentFrame(u32 a, u32 b)
{
    if (a == 0)
        return 0;
    else {
        u32 *p = gUnk_02000610.argSp;
        u32 *q = p + b;
        *q++ = a;
        gUnk_02000610.argSp = q;
        return p;
    }
}

void Script_RestoreArgumentStack(u32 *p)
{
    if (p)
        gUnk_02000610.argSp = p;
}

u32 Script_GetCallArgument(s32 n)
{
    if (!n)
        return gUnk_02000610.result;
    else {
        u32 *e = (u32 *)gUnk_02000610.argSp[-1];
        return ((u32 *)e[1])[n - 1];
    }
}

u32 Script_GetStackArgument(s32 n)
{
    return *(u32 *)((u8 *)gUnk_02000610.argSp - (n * 4 + 4));
}

void Script_SetStackArgument(s32 n, u32 v)
{
    *(u32 *)((u8 *)gUnk_02000610.argSp - (n * 4 + 4)) = v;
}

void Script_ResetKeywordStack(void)
{
    gUnk_02000610.keywordSp = gUnk_02000610.stack;
}

void Script_PushCtrlNextKeyword(u32 v)
{
    *gUnk_02000610.keywordSp++ = v;
}

void Script_PopCtrlNextKeyword(void)
{
    gUnk_02000610.keywordSp--;
}

void Script_SetPc(u8 *p)
{
    gUnk_02000610.pc = p;
}

/* engine.h declares this s32(s32); the real parameter is a byte (the prologue
 * zero-extends it) -- narrow it explicitly so the body still matches. */

s32 Script_SeekToKeyword(s32 keyword)
{
    u8 k = keyword;
    s32 a, b;
    u8 *p = (u8 *)gUnk_02000610.keywordSp[-1];
    for (;;) {
        p = Script_DecodeOperand(p, &a, &b);
        if (a == 0)
            return 0;
        if ((a & 0xF0) == 0x50 && (a >> 16) == k) {
            gUnk_02000610.pc = (u8 *)b;
            return b;
        }
    }
}

s32 Script_ReadNextKeyword(void)
{
    s32 a, b;
    u8 *p = gUnk_02000610.pc;
    if (p == 0)
        return 0;
    if (*p == 0)
        return 0;
    for (;;) {
        p = Script_DecodeOperand(p, &a, &b);
        if (a == 0)
            return 0;
        if ((a & 0xF0) != 0x50)
            continue;
        gUnk_02000610.pc = (u8 *)b;
        return a >> 16;
    }
}

s32 Script_ReadValue(const void *p)
{
    s32 a, b;
    gUnk_02000610.pc = Script_DecodeOperand((u8 *)p, &a, &b);
    return b;
}

s32 Script_ReadVector32(u8 *p, s32 *destination)
{
    s32 type, value;
    s32 *vector;
    s32 count;
    vector = destination;
    for (count = 0; count < 3; count++) {
        p = Script_DecodeOperand(p, &type, &value);
        vector[count] = value;
    }
    gUnk_02000610.pc = p;
    return 0;
}

s32 Script_ReadVector16(u8 *p, s16 *destination)
{
    s32 type, value;
    s16 *vector;
    s32 count;
    vector = destination;
    for (count = 0; count < 3; count++) {
        p = Script_DecodeOperand(p, &type, &value);
        vector[count] = value;
    }
    gUnk_02000610.pc = p;
    return 0;
}

u32 Script_ReadValueSafe(u8 *p)
{
    s32 a, b;
    if (p) {
        gUnk_02000610.pc = Script_DecodeOperand(p, &a, &b);
        if (gUnk_02000610.pc)
            return b;
    }
    return 0;
}

u32 Script_ReadString(u8 *p)
{
    return Script_ReadValueSafe(p);
}

s32 Script_ParseStringRef(const void *pc)
{
    const u8 *p = pc;
    s32 v = (s16)((p[2] << 8) | p[1]);
    gUnk_02000610.pc = (u8 *)p + 3;
    return v;
}

void Script_ReadReference(struct ScriptUtilityReference *ref)
{
    gUnk_02000610.pc = Script_DecodeVariableReference(Script_GetPc(), ref);
}

/* Gap: scans operands from a hidden cursor argument until the terminating zero
 * type and clears the PC. gbadisasm left it as data; no caller (bl or pointer)
 * has been found yet. See symbols/progress_functions.csv. */
u8 *sub_0821AAF4(u8 *p)
{
    s32 type, value;
    u8 **pc;
    do {
        p = Script_DecodeOperand(p, &type, &value);
        pc = &gUnk_02000610.pc;
    } while (type != 0);
    *pc = (u8 *)type;
    return p;
}

u8 *Script_GetPc(void)
{
    u8 *p = gUnk_02000610.pc;
    if (p == 0 || *p == 0 || (*p & 0xF0) == 0x50)
        return 0;
    return p;
}

s32 Script_GetValue(void)
{
    return Script_ReadValue(Script_GetPc());
}

/* Script_GetVector32: Reads a three component thirty two bit vector from the current script PC */

#define Script_GetVector32 sub_0821AB50

s32 sub_0821AB50(void *p)
{
    return Script_ReadVector32(Script_GetPc(), p);
}

/* Script_GetVector: Decodes three operands at the current script cursor into a signed 16-bit vector */

#define Script_GetVector sub_0821AB64

s32 sub_0821AB64(void *p)
{
    return Script_ReadVector16(Script_GetPc(), p);
}

/* Same body as Script_GetValueSafe (0821AB88); kept as a separate ROM function. */

u32 sub_0821AB78(void)
{
    return Script_ReadValueSafe(Script_GetPc());
}

u32 Script_GetValueSafe(void)
{
    return Script_ReadValueSafe(Script_GetPc());
}

/* Script_GetStringRef: Parses a string reference at the current script cursor and advances it */

#define Script_GetStringRef sub_0821AB98

s32 sub_0821AB98(void)
{
    return Script_ParseStringRef(Script_GetPc());
}

s32 Script_GetKeywordValue(s32 keyword, s32 defaultValue)
{
    u8 k = keyword;
    if (Script_SeekToKeyword(k) != 0)
        return Script_ReadValue(Script_GetPc());
    return defaultValue;
}

void Script_ResetStacks(void)
{
    Script_ResetArgumentStack();
    Script_ResetKeywordStack();
}

void Script_ClearCommandTables(void)
{
    gUnk_02000458 = 0;
}

u32 Script_RegisterCommandTable(struct ScriptCommandTable *n)
{
    n->next = gUnk_02000458;
    gUnk_02000458 = n;
    return 0;
}

/* Gap: unlinks a command table from the registered list, returning 0 if found in
 * the middle, -1 if it was the head or not found. gbadisasm left it as data; no
 * caller (bl or pointer) has been found yet. See symbols/progress_functions.csv. */
s32 sub_0821ABF8(struct ScriptCommandTable *n)
{
    struct ScriptCommandTable *cur;
    struct ScriptCommandTable *prev;
    if (gUnk_02000458 == n)
        gUnk_02000458 = n->next;
    cur = gUnk_02000458;
    prev = cur;
    while (cur != 0) {
        if (cur == n) {
            prev->next = cur->next;
            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    return -1;
}

struct ScriptCommandEntry *Script_FindCommand(u32 key)
{
    struct ScriptCommandTable *b = gUnk_02000458;
    while (b) {
        struct ScriptCommandEntry *e = b->entries;
        s32 n = b->count;
        for (; n > 0; e++, n--)
            if (e->key == key)
                return e;
        b = b->next;
    }
    return 0;
}

u8 *Script_ExecuteNativeCommand(u8 *p)
{
    u32 len;
    u32 id = (p[1] << 8) | p[0];
    struct ScriptCommandEntry *e;
    p += 2;
    e = Script_FindCommand(id);
    p = Script_DecodeCommandOffset(p, &len);
    Script_PushCtrlNextKeyword((u32)(p + len));
    Script_SetPc(p);
    p = Callback_InvokeActor(p, e->callback);
    Script_PopCtrlNextKeyword();
    return p;
}

u32 *Script_ReadProcTable(u32 *p, u32 *n)
{
    s32 c;
    u32 *q;
    u32 *o = n;
    c = 0;
    q = p;
    while (*q != (u32)-1) {
        q++;
        c++;
    }
    *o = c;
    return q + 1;
}

u8 *Script_GetProcedure(u32 id, u32 *out)
{
    u32 idx = (id & 0x7FFFFFFF) - 1;
    struct ScriptProcedureTable *t = &gUnk_02000438;
    u32 *e = t->entries + idx;
    *out = ((u8 *)e)[3];
    return t->body + (e[0] & 0xFFFFFF);
}

s32 Script_Call(u32 id, void *b)
{
    u32 x, y;
    return Script_RunProcedureBlock(Script_DecodePacket(Script_GetProcedure(id, &x), &y), b, x);
}

s32 Script_CallInlineProcedure(u8 *p)
{
    s32 values[16], type, value;
    struct ScriptUtilityArguments args;
    s32 high = p[1] << 8;
    s32 id = (s16)(p[0] | high);
    s32 count;
    p += 2;
    count = 0;
    for (;;) {
        p = Script_DecodeOperand(p, &type, &value);
        if (!type)
            break;
        values[count++] = value;
    }
    args.count = count;
    args.values = values;
    return Script_Call(id, &args);
}

/* Gap: a binary search by key over a ScriptArchiveEntry table, returning the entry
 * pointer (not its value, unlike Script_LookupArchive). gbadisasm left it as data;
 * no caller (bl or pointer) has been found yet. See symbols/progress_functions.csv. */
void *sub_0821AD98(void *table, u32 key, s32 count)
{
    struct ScriptArchiveEntry *tbl = table;
    s32 lo = 0, hi = count - 1;
    s32 mid;
    struct ScriptArchiveEntry *e;
    while (lo < hi) {
        mid = (lo + hi) >> 1;
        if (tbl[mid].key < key)
            lo = mid + 1;
        else
            hi = mid;
    }
    e = (struct ScriptArchiveEntry *)((lo << 3) + (u32)tbl);
    if (e->key == key)
        return e;
    return 0;
}

u8 *Text_LookupString(s32 id)
{
    u8 **sections = gUnk_02000448;
    u8 *e = (u8 *)((id << 2) + (u32)sections[1]);
    u32 v = (e[3] << 24) | (e[2] << 16) | (e[1] << 8) | e[0];
    return sections[2] + (v & 0x7FFFFFFF);
}

extern u32 gUnk_0200060C;

static inline u32 read_word(u8 *p)
{
    return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
}

s32 Script_LoadScript(u8 *p)
{
    u32 count;
    struct ScriptProcedureTable *ctx = &gUnk_02000438;
    u8 *base;
    gUnk_0200060C = read_word(p);
    p += 4;
    base = (u8 *)Script_ReadProcTable((u32 *)p, &count);
    ctx->entries = (u32 *)p;
    ctx->count = count;
    gUnk_02000448[0] = base;
    gUnk_02000448[1] = base + read_word(base + 4);
    gUnk_02000448[2] = base + read_word(base + 8);
    gUnk_02000448[3] = base + read_word(base + 12);
    base += read_word(base);
    ctx->body = base + 4;
    {
        u32 size = read_word(base);
        size += (u32)base;
        size += 8;
        ctx->mainBlock = (u8 *)size;
    }
    return 0;
}

/* Script_GetSectionThreeData: Returns data after the fourth resolved section pointer's header with section purpose tentative */

#define Script_GetSectionThreeData sub_0821AED0

u8 *sub_0821AED0(void)
{
    return gUnk_02000448[3] + 4;
}

/* Script_SaveContext: Copies the loaded script context and section pointers into caller storage */

#define Script_SaveContext sub_0821AEDC

struct Block0821AEDC {
    u32 w[4];
};

void sub_0821AEDC(struct Block0821AEDC *dst)
{
    dst[0] = *(struct Block0821AEDC *)&gUnk_02000438;
    dst[1] = *(struct Block0821AEDC *)&gUnk_02000448;
}

/* Script_RestoreContext: Restores the loaded script context and section pointers from caller storage */

#define Script_RestoreContext sub_0821AF04

struct Block0821AF04 {
    u32 w[4];
};

void sub_0821AF04(struct Block0821AF04 *src)
{
    gUnk_02000438 = *(struct ScriptProcedureTable *)&src[0];
    *(struct Block0821AF04 *)gUnk_02000448 = src[1];
}

u32 Script_ExecuteBlock(u8 *p, u32 a, u32 b)
{
    u32 length;
    u32 *saved;
    saved = Script_PushArgumentFrame(a, b);
    while (p) {
        switch (*p & 0xF0) {
            case 0x60:
                p = Script_DecodePacket(p, &length);
                if (Script_ExecuteNativeCommand(p) == 1) {
                    p = (u8 *)1;
                    goto done;
                }
                p += length;
                break;
            case 0x70:
                p = Script_DecodePacket(p, &length);
                gUnk_02000610.result = Script_CallInlineProcedure(p);
                p += length;
                break;
            case 0x30:
                p = Script_DecodePacket(p, &length);
                gUnk_02000610.result = Script_EvalExpression(p);
                p += length;
                break;
            case 0:
                goto stop;
        }
    }
stop:
    p = 0;
done:
    Script_RestoreArgumentStack(saved);
    return (u32)p;
}

u32 Script_RunBlock(u32 a, struct ScriptUtilityArguments *b)
{
    if (Script_ExecuteBlock((u8 *)a, (u32)b, 0) == 1)
        return gUnk_02000610.result;
    return gUnk_02000610.result = 0;
}

extern const u8 gUnk_08614184[];

u32 Script_RunProcedureBlock(u32 a, const u8 *b, u32 c)
{
    if (b == 0)
        b = gUnk_08614184;
    if (Script_ExecuteBlock((u8 *)a, (u32)b, c) == 1)
        return gUnk_02000610.result;
    return gUnk_02000610.result = 0;
}

void Script_RunMainBlock(void)
{
    u32 x;
    Script_RunBlock((u32)Script_DecodePacket(gUnk_02000438.mainBlock, &x),
                    (struct ScriptUtilityArguments *)gUnk_08614184);
}

s32 Script_DiscardValueIfZero(void)
{
    if (Script_GetValue() == 0)
        sub_0821AB78();
    return 0;
}

extern u16 gUnk_030039F0;

void Script_SetEntryProcedure(u16 v)
{
    gUnk_030039F0 = v;
}

void Script_InitRuntime(void)
{
    Script_ResetStacks();
    GameState_ClearActiveRegions();
    Script_ClearCommandTables();
    Script_RegisterBuiltinCommands();
    ScriptProc_Start(0xA8D9, 0x49BC);
    /* Falls through: r0 (ScriptProc_Start's resource pointer) becomes Script_LoadScript's
     * argument. The cast avoids a hard arg-count mismatch against its real prototype now
     * that both are in the same translation unit. */
    ((s32(*)())Script_LoadScript)();
    Script_SetEntryProcedure(0);
}

void Script_ResetLocalVariables(void)
{
    Script_ClearLocalVariables();
}

// Provisional name: GameState_SelectRamBase

/* Select timer-jittered storage for the game-state regions. */

extern u8 gUnk_0203C400[];

// MATCHING_CONCESSION: register-pinning
void GameState_SelectRamBase(void)
{
    u32 timer = *(vu16 *)0x0400010C;
    register u8 *base asm("r1") = gUnk_0203C400;
    base += (timer & 127) * 8;
    gUnk_02000710 = (struct GameState *)base;
    gUnk_0200070C = base + 0xb00;
    gUnk_02000708 = base + 0x1600;
    gUnk_02000704 = base + 0x1a00;
    gUnk_02000700 = base + 0x1e00;
}

void GameState_ClearActiveRegions(void)
{
    Memory_Clear(gUnk_02000708, 0x400);
    Memory_Clear(gUnk_02000710, 0x89C);
}

void GameState_ResetActiveRegions(void)
{
    u16 v = *(u16 *)((u8 *)gUnk_02000710 + 0x744);
    Memory_Clear(gUnk_02000708, 0x400);
    Memory_Clear((u8 *)gUnk_02000710 + 0x30, 0x74A);
    *(u16 *)((u8 *)gUnk_02000710 + 0x744) = v;
    GameState_BackupActiveRegions();
}

void GameState_BackupActiveRegions(void)
{
    Memory_Copy(gUnk_0200070C, gUnk_02000710, 0x89C);
    Memory_Copy(gUnk_02000704, gUnk_02000708, 0x400);
}

void GameState_RestoreActiveRegions(void)
{
    Memory_Copy(gUnk_02000710, gUnk_0200070C, 0x89C);
    Memory_Copy(gUnk_02000708, gUnk_02000704, 0x400);
}

void GameState_BackupRegionRange(u8 *p, u32 n)
{
    s32 d = p - (u8 *)gUnk_02000710;
    Memory_Copy(gUnk_0200070C + d, p, n);
}

u8 *GameState_GetBackupRegionPointer(u8 *p)
{
    s32 d = p - (u8 *)gUnk_02000710;
    return gUnk_0200070C + d;
}

void Script_ClearLocalVariables(void)
{
    Memory_Clear(gUnk_02000700, 0x400);
}

struct ScriptShortArrayView {
    s16 elements[1];
};

void Script_ReadTypedVariable(u8 *p, s32 descriptor, s32 index, s32 *value)
{
    switch ((descriptor >> 24) & 15) {
        case 9:
            p += index * 4;
            *value = (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
            break;
        case 8:
            p += index * 4;
            *value = (p[1] << 8) | p[0];
            break;
        case 1:
        case 6:
            *value = ((struct ScriptShortArrayView *)p)->elements[index];
            break;
        case 2:
        case 3:
            p += index;
            *value = *p;
            break;
        case 4: {
            u8 *byte;
            u32 mask, bits;
            index += (descriptor >> 16) & 15;
            byte = p + (index >> 3);
            mask = 1u << (index & 7);
            bits = *byte & mask;
            *value = bits != 0;
            break;
        }
    }
}

u8 *Script_DecodeVariableValue(u8 *p, s32 *type, s32 *value)
{
    s32 ignored, first, index;
    u32 descriptor = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    u8 *base, *end;
    *type = (descriptor >> 24) & 15;
    if ((descriptor & 0xF00000) == 0x800000)
        base = (u8 *)gUnk_02000710;
    else if ((descriptor & 0xF00000) == 0x100000)
        base = gUnk_02000700;
    else
        base = gUnk_02000708;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20) {
        end = Script_DecodeOperand(p + 4, &ignored, &first);
        end = Script_DecodeOperand(end, &ignored, &index);
    } else {
        index = 0;
        end = p + 4;
    }
    Script_ReadTypedVariable(base, descriptor, index, value);
    return end;
}

struct ScriptWordArrayView {
    s32 elements[1];
};

void Script_WriteTypedVariable(u8 *p, s32 descriptor, s32 index, s32 value)
{
    switch ((descriptor >> 24) & 15) {
        case 9:
            ((struct ScriptWordArrayView *)p)->elements[index] = value;
            break;
        case 8: {
            u8 *q = p + index * 4;
            q[2] = value >> 16;
            q[1] = value >> 8;
            q[0] = value;
            break;
        }
        case 1:
        case 6:
            ((struct ScriptShortArrayView *)p)->elements[index] = value;
            break;
        case 2:
        case 3:
            p[index] = value;
            break;
        case 4: {
            s32 mask;
            index += (descriptor >> 16) & 15;
            p += index >> 3;
            mask = 1 << (index & 7);
            if (value)
                *p |= mask;
            else
                *p &= ~mask;
            break;
        }
    }
}

u8 *Script_AssignVariableOperand(u8 *p, s32 value)
{
    s32 ignored, first, index;
    u32 descriptor = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    u8 *base, *end;
    if ((descriptor & 0xF00000) == 0x800000)
        base = (u8 *)gUnk_02000710;
    else if ((descriptor & 0xF00000) == 0x100000)
        base = gUnk_02000700;
    else
        base = gUnk_02000708;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20) {
        end = Script_DecodeOperand(p + 4, &ignored, &first);
        end = Script_DecodeOperand(end, &ignored, &index);
    } else {
        index = 0;
        end = p + 4;
    }
    Script_WriteTypedVariable(base, descriptor, index, value);
    return end;
}

u8 *Script_DecodeVariableReference(u8 *p, struct ScriptUtilityReference *ref)
{
    s32 type, count, index;
    u8 *end;
    Memory_Copy(ref->descriptor, p, 4);
    if ((ref->descriptor[0] & 0xF0) == 0x20) {
        end = Script_DecodeOperand(p + 4, &type, &count);
        end = Script_DecodeOperand(end, &type, &index);
        ref->count = count;
        ref->index = index;
    } else {
        end = p + 4;
        ref->count = 1;
        ref->index = 0;
    }
    return end;
}

void Script_WriteReferenceValue(struct ScriptUtilityReference *ref, s32 index, s32 value)
{
    u32 descriptor = (ref->descriptor[0] << 24) | (ref->descriptor[1] << 16) |
                     (ref->descriptor[2] << 8) | ref->descriptor[3];
    u8 *base;
    if ((descriptor & 0xF00000) == 0x800000)
        base = (u8 *)gUnk_02000710;
    else if ((descriptor & 0xF00000) == 0x100000)
        base = gUnk_02000700;
    else
        base = gUnk_02000708;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20)
        index += ref->index;
    Script_WriteTypedVariable(base, descriptor, index, value);
}

s32 Script_ReadReferenceValue(struct ScriptUtilityReference *ref, s32 index)
{
    s32 value;
    u32 descriptor = (ref->descriptor[0] << 24) | (ref->descriptor[1] << 16) |
                     (ref->descriptor[2] << 8) | ref->descriptor[3];
    u8 *base;
    if ((descriptor & 0xF00000) == 0x800000)
        base = (u8 *)gUnk_02000710;
    else if ((descriptor & 0xF00000) == 0x100000)
        base = gUnk_02000700;
    else
        base = gUnk_02000708;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20)
        index += ref->index;
    Script_ReadTypedVariable(base, descriptor, index, &value);
    return value;
}

u8 *Script_BackupVariableOperand(u8 *p)
{
    struct ScriptUtilityReference ref;
    u8 *r = (u8 *)&ref;
    u8 *end, *base;
    s32 index = 0;
    s32 value;
    u32 descriptor;
    end = Script_DecodeVariableReference(p, &ref);
    value = Script_ReadReferenceValue(&ref, 0);
    descriptor = (r[0] << 24) | (r[1] << 16) | (r[2] << 8) | r[3];
    if ((descriptor & 0xF00000) == 0x800000)
        base = gUnk_0200070C;
    else
        base = gUnk_02000704;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20)
        index += *(u16 *)(r + 6);
    Script_WriteTypedVariable(base, descriptor, index, value);
    return end;
}

s32 Script_ReadBackupVariableOperand(u8 *p)
{
    struct ScriptUtilityReference ref;
    u8 *r = (u8 *)&ref;
    s32 value;
    u8 *base;
    s32 index = 0;
    u32 descriptor;
    Script_DecodeVariableReference(p, &ref);
    descriptor = (r[0] << 24) | (r[1] << 16) | (r[2] << 8) | r[3];
    if ((descriptor & 0xF00000) == 0x800000)
        base = gUnk_0200070C;
    else
        base = gUnk_02000704;
    base += descriptor & 0xFFFF;
    if (((descriptor >> 24) & 0xF0) == 0x20)
        index += *(u16 *)(r + 6);
    Script_ReadTypedVariable(base, descriptor, index, &value);
    return value;
}

u8 *GameState_GetBackupStateBase(void)
{
    return gUnk_0200070C;
}

u8 *GameState_GetBackupVariableBase(void)
{
    return gUnk_02000704;
}

s32 Div(s32, s32);

void Script_ConvertTextToShiftJis(u8 *output, u8 *input)
{
    u8 *src = input;
    u8 *dest = output;
    while (*src) {
        if (*src & 0x80) {
            s32 row = *src - 0x80;
            s32 column = src[1];
            s32 adjusted;
            src++;
            if (row & 1) {
                adjusted = column - 0x61;
                if (adjusted > 0x7E)
                    adjusted++;
            } else
                adjusted = column - 2;
            row = Div(row - 0x21, 2) + 0x81;
            if (row > 0x9F)
                row += 0x40;
            dest[0] = row;
            dest[1] = adjusted;
            dest++;
        } else
            *dest = *src;
        if (src[0] == 13 && src[1] == 10)
            src++;
        src++;
        dest++;
    }
    *dest = 0;
}

u8 *Script_SelectConditionalBlock(u8 *p)
{
    s32 type, condition, block;
condition_next:
    p = Script_DecodeOperand(p, &type, &condition);
block_next:
    p = Script_DecodeOperand(p, &type, &block);
    if (condition)
        return (u8 *)block;
    p = Script_DecodeOperand(p, &type, &condition);
    if (p) {
        type >>= 16;
        p = (u8 *)condition;
        if (type == 'e') {
            condition = 1;
            goto block_next;
        }
        if (type == 'i')
            goto condition_next;
    }
    return 0;
}

u32 Script_ExecuteIf(u8 *p)
{
    u8 *r = Script_SelectConditionalBlock(p);
    if (r != 0)
        return Script_ExecuteBlock(r, 0, 0);
    return 0;
}

s32 Script_ExecuteSwitch(u8 *p)
{
    s32 type, value, block = 0;
    Script_SetPc(Script_DecodeOperand(p, &type, &value));
    for (;;) {
        s32 option = Script_ReadNextKeyword();
        if (!option)
            return 0;
        if (option == 'c') {
            if (Script_GetValue() != value)
                continue;
            Script_DecodeOperand(Script_GetPc(), &type, &block);
            break;
        }
        if (option == 'd') {
            Script_DecodeOperand(Script_GetPc(), &type, &block);
            break;
        }
    }
    return Script_ExecuteBlock((u8 *)block, 0, 0);
}

s32 Script_SetResultFromOperand(u8 *p)
{
    s32 a, b;
    Script_DecodeOperand(p, &a, &b);
    gUnk_02000610.result = b;
    return 0;
}

s32 Script_ExecuteReturn(void)
{
    u8 *p = Script_GetPc();
    if (p)
        gUnk_02000610.result = Script_GetValue();
    else
        gUnk_02000610.result = (u32)p;
    return 1;
}

s32 Script_ExecutePrint(u8 *p)
{
    u8 buffer[512];
    s32 type, value;
    while (p) {
        p = Script_DecodeOperand(p, &type, &value);
        if (!type)
            break;
        if (type == 7)
            Script_ConvertTextToShiftJis(buffer, (u8 *)value);
        else if (type == 14) {
            u8 *q = (u8 *)value;
            while (*q)
                q++;
        }
    }
    return 0;
}

s32 Script_CallFromOperands(void)
{
    s32 values[16], type, value;
    struct ScriptUtilityArguments args;
    s32 id = Script_GetValue();
    u8 *p = Script_GetPc();
    s32 count = 0;
    while (p) {
        p = Script_DecodeOperand(p, &type, &value);
        if (!type)
            break;
        values[count++] = value;
    }
    args.count = count;
    args.values = values;
    return Script_Call(id, &args);
}

extern struct ScriptCommandTable gUnk_02000428;

extern const u8 gUnk_0861418C[];

void Script_RegisterBuiltinCommands(void)
{
    gUnk_02000428.next = 0;
    gUnk_02000428.count = Div(0x30, 8);
    gUnk_02000428.entries = (struct ScriptCommandEntry *)gUnk_0861418C;
    Script_RegisterCommandTable(&gUnk_02000428);
}

s32 Mod(s32, s32);

s32 Script_ApplyOperator(u32 operation, s32 left, s32 right)
{
    switch (operation) {
        case 1:
            return -right;
        case 2:
            return !right;
        case 3:
            return ~right;
        case 4:
            return left + right;
        case 5:
            return left - right;
        case 6:
            return left * right;
        case 7:
            return Div(left, right);
        case 8:
            return Mod(left, right);
        case 9:
            left <<= right;
            return left;
        case 10:
            left = (u32)left >> right;
            return left;
        case 11:
            return left == right;
        case 12:
            return left != right;
        case 13:
            return left < right;
        case 14:
            return left <= right;
        case 15:
            return left > right;
        case 16:
            return left >= right;
        case 17:
            left |= right;
            return left;
        case 18:
            left &= right;
            return left;
        case 19:
            left ^= right;
            return left;
        case 20:
            return left || right;
        case 21:
            return left && right;
        case 23:
            return right;
        default:
            return 0;
    }
}

struct Slot {
    s32 value;
    u8 *reference;
};

struct Frame {
    struct Slot stack[8];
    s32 type, value;
};

u32 Script_EvalExpression(u8 *p)
{
    struct Frame frame;
    struct Slot *slot = (struct Slot *)((u32)frame.stack - 8);
    for (;;) {
        frame.type = *p;
        if ((frame.type & 0xE0) == 0xA0) {
            s32 op = frame.type & ~0xE0;
            slot = (struct Slot *)((u32)slot - 8);
            if (!op)
                return ((struct Slot *)((u32)slot + 8))->value;
            if (op == 0x16) {
                struct Slot *saved;
                u8 *reference = slot->reference;
                u32 type = *reference;
                u32 category = type & 0xF0;
                saved = slot;
                if (category == 0x90)
                    Script_SetStackArgument(type & 15, ((struct Slot *)((u32)slot + 8))->value);
                else
                    Script_AssignVariableOperand(reference,
                                                 ((struct Slot *)((u32)slot + 8))->value);
                saved->value = ((struct Slot *)((u32)slot + 8))->value;
            } else {
                slot->value =
                    Script_ApplyOperator(op, slot->value, ((struct Slot *)((u32)slot + 8))->value);
                slot->reference = 0;
            }
            p++;
        } else {
            ((struct Slot *)((u32)slot + 8))->reference = p;
            p = Script_DecodeOperand(p, &frame.type, &frame.value);
            if (frame.type == 0x80) {
                Script_ExecuteBlock((u8 *)frame.value, 0, 0);
                ((struct Slot *)((u32)slot + 8))->value = gUnk_02000610.result;
            } else
                ((struct Slot *)((u32)slot + 8))->value = frame.value;
            slot = (struct Slot *)((u32)slot + 8);
        }
    }
}
