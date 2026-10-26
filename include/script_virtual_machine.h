#ifndef GUARD_SCRIPT_VIRTUAL_MACHINE_H
#define GUARD_SCRIPT_VIRTUAL_MACHINE_H

/*
 * The GCL bytecode interpreter: operand decoding, the argument/keyword stacks,
 * native command dispatch, procedure lookup and the postfix expression
 * evaluator. Built from field offsets seen across src/fn/*.c; names are
 * provisional. See docs/ASSETS.md for the bytecode format.
 */

#include "global.h"
#include "engine.h"
#include "script_reference.h"
#include "script_utility.h"

/* Resolved section base pointers from the loaded script header (gUnk_02000448):
 * 0 unknown/adjacent to the proc table, 1 string offset table (4-byte records,
 * see Text_LookupString), 2 string data base, 3 a fourth resource (guess). */
extern u8 *gUnk_02000448[4];

/* The currently loaded script's procedure table (gScriptProcedureTable). `entries` holds
 * one packed word per procedure: low 24 bits an offset from `body`, high byte
 * metadata (returned by Script_GetProcedure). `mainBlock` is the bytecode run
 * when no entry procedure is selected (Script_RunMainBlock). */
struct ScriptProcedureTable {
    u32 *entries;
    u32 count;
    u8 *body;
    u8 *mainBlock;
};
extern struct ScriptProcedureTable gScriptProcedureTable;

/* A registered table of native script commands (gUnk_02000458 is the head of
 * the list). Script_ExecuteNativeCommand looks a command id up by key and
 * invokes its callback. */
struct ScriptCommandEntry {
    u32 key;
    void *callback;
};
struct ScriptCommandTable {
    struct ScriptCommandTable *next;
    s32 count;
    struct ScriptCommandEntry *entries;
};
extern struct ScriptCommandTable *gUnk_02000458;

/* Interpreter state (gUnk_02000610). `stack` backs both the keyword/control
 * stack (growing up from its base, via keywordSp) and the call argument stack
 * (growing up from stack + 25, via argSp) -- the two are never deep at once. */
struct ScriptVm {
    u8 *pc;
    u32 result;
    u32 *keywordSp;
    u32 stack[24];
    u32 *argSp;
};
extern struct ScriptVm gUnk_02000610;

/* ---- Resource lookup (archive keyed by extension/name hash pair) -------- */

s32 Script_FindResourceKey(u32 key, u16 *keys, s32 context, s32 lo, s32 hi);
u8 *Script_LookupResource(u8 *base, u32 unused, u16 key, u16 context);

struct ScriptArchiveEntry {
    u32 key, value;
};
u32 Script_LookupArchive(u32 key, struct ScriptArchiveEntry *table, s32 lo, s32 hi);

/* ---- Operand and packet decoding ---------------------------------------- */

u8 *Script_DecodeOperand(u8 *p, s32 *type, s32 *value);
u8 *Script_DecodePacket(u8 *p, u32 *length);
u8 *Script_DecodeCommandOffset(u8 *p, u32 *offset);

/* ---- Argument stack (call arguments for the current procedure) --------- */

void Script_ResetArgumentStack(void);
u32 *Script_PushArgumentFrame(u32 args, u32 count);
void Script_RestoreArgumentStack(u32 *saved);
u32 Script_GetCallArgument(s32 index);
u32 Script_GetStackArgument(s32 index);
void Script_SetStackArgument(s32 index, u32 value);

/* ---- Keyword/control stack (nested command and if/switch scopes) ------- */

void Script_ResetKeywordStack(void);
void Script_PushCtrlNextKeyword(u32 pc);
void Script_PopCtrlNextKeyword(void);

void Script_ResetStacks(void);

/* ---- Script cursor (gUnk_02000610.pc) ----------------------------------- */

void Script_SetPc(u8 *p);
/* Script_SeekToKeyword, Script_GetPc, Script_ParseStringRef, Script_GetKeywordValue: engine.h */
s32 Script_ReadNextKeyword(void);

/* ---- Value and reference reading at the cursor -------------------------- */

/* Script_ReadValue: script_utility.h */
s32 Script_ReadVector32(u8 *p, s32 *destination);
s32 Script_ReadVector16(u8 *p, s16 *destination);
u32 Script_ReadValueSafe(u8 *p);
u32 Script_ReadString(u8 *p);
void Script_ReadReference(struct ScriptUtilityReference *ref);

u32 Script_GetValueSafe(void);

/* ---- Native command tables ----------------------------------------------- */

void Script_ClearCommandTables(void);
u32 Script_RegisterCommandTable(struct ScriptCommandTable *table);
struct ScriptCommandEntry *Script_FindCommand(u32 key);
u8 *Script_ExecuteNativeCommand(u8 *p);
void Script_RegisterBuiltinCommands(void);

/* ---- Procedures ----------------------------------------------------------- */

u32 *Script_ReadProcTable(u32 *p, u32 *count);
u8 *Script_GetProcedure(u32 id, u32 *metadata);
s32 Script_LoadScript(u8 *p);

/* ---- Block execution -------------------------------------------------------- */

u32 Script_ExecuteBlock(u8 *p, u32 args, u32 count);
/* Script_RunBlock: script_utility.h */
u32 Script_RunProcedureBlock(u32 a, const u8 *b, u32 c);
void Script_RunMainBlock(void);
s32 Script_DiscardValueIfZero(void);
void Script_SetEntryProcedure(u16 id);
void Script_InitRuntime(void);
void Script_ResetLocalVariables(void);

/* ---- Game-state and script-variable regions ------------------------------ */

void GameState_SelectRamBase(void);
void GameState_ClearActiveRegions(void);
void GameState_ResetActiveRegions(void);
void GameState_BackupActiveRegions(void);
void GameState_RestoreActiveRegions(void);
void GameState_BackupRegionRange(u8 *p, u32 n);
u8 *GameState_GetBackupRegionPointer(u8 *p);
u8 *GameState_GetBackupStateBase(void);
u8 *GameState_GetBackupVariableBase(void);

void Script_ClearLocalVariables(void);

/* ---- Script variables ------------------------------------------------------ */

void Script_ReadTypedVariable(u8 *base, s32 descriptor, s32 index, s32 *value);
void Script_WriteTypedVariable(u8 *base, s32 descriptor, s32 index, s32 value);
u8 *Script_DecodeVariableValue(u8 *p, s32 *type, s32 *value);
u8 *Script_AssignVariableOperand(u8 *p, s32 value);
u8 *Script_DecodeVariableReference(u8 *p, struct ScriptUtilityReference *ref);
void Script_WriteReferenceValue(struct ScriptUtilityReference *ref, s32 index, s32 value);
s32 Script_ReadReferenceValue(struct ScriptUtilityReference *ref, s32 index);
u8 *Script_BackupVariableOperand(u8 *p);
s32 Script_ReadBackupVariableOperand(u8 *p);

/* ---- Control flow commands -------------------------------------------------- */

void Script_ConvertTextToShiftJis(u8 *output, u8 *input);
u8 *Script_SelectConditionalBlock(u8 *p);
u32 Script_ExecuteIf(u8 *p);
s32 Script_ExecuteSwitch(u8 *p);
s32 Script_SetResultFromOperand(u8 *p);
s32 Script_ExecuteReturn(void);
s32 Script_ExecutePrint(u8 *p);
s32 Script_CallFromOperands(void);

s32 Script_ApplyOperator(u32 operation, s32 left, s32 right);
u32 Script_EvalExpression(u8 *p);

#endif
