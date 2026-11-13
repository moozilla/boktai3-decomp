"""Correct discarded-return declarations using verified existing C definitions.

Only explicit void declarations are changed. Calls already discard the result,
so this does not invent arguments, values, or behavior. Parameter reconciliation
and indirect callback types remain separate tasks.
"""
import re

# Function: (actual return spelling, original definition source).
RETURNS = {
    'Task_Kill': ('s32', 'src/fn/sub_0821A0C0.c'),
    'Script_Call': ('s32', 'src/script_virtual_machine.c'),
    'Trigger_ConsumePending': ('u32', 'src/fn/sub_08020D68.c'),
    'Widget_InitLinked': ('s32', 'src/fn/sub_0821983C.c'),
    'Actor_PlayAnimationEntry': ('int', 'src/fn/sub_0802DCD8.c'),
    'AnimData_Load': ('s32', 'src/fn/sub_082151E4.c'),
    'Anim_SetFrameDurationScale': ('s32', 'src/fn/sub_08220F00.c'),
    'DamageNumberDisplay_Show': ('s32', 'src/fn/sub_080472A4.c'),
    'Anim_RestartSequence': ('u32', 'src/fn/sub_08220D20.c'),
    'SparkParticle_Spawn': ('s32', 'src/fn/sub_0806A160.c'),
    'Anim_LoadKeyframe': ('s32', 'src/fn/sub_0821980C.c'),
    'TextWindow_StartPlayback': ('s32', 'src/fn/sub_080337FC.c'),
    'MenuDisplay_UpdateActionPrompt': ('s32', 'src/fn/sub_08166B7C.c'),
    'Anim_SetSequence': ('u32', 'src/fn/sub_08220C8C.c'),
    'ScrollingDialogue_ClearScript': ('u32', 'src/fn/sub_08033468.c'),
    'ResourceHeader_Relocate': ('s32', 'src/fn/sub_082196C4.c'),
    'TextWindow_Refresh': ('s32', 'src/fn/sub_08033924.c'),
    'SceneBody_Unregister': ('s32', 'src/fn/sub_08225938.c'),
    'Anim_Step': ('s32', 'src/fn/sub_08220F70.c'),
    'SceneBody_Init': ('u32', 'src/fn/sub_08225AB4.c'),
    'MotionTrack_Update': ('s32', 'src/fn/sub_081B95E4.c'),
    'Shadow_Destroy': ('u32', 'src/fn/sub_08013684.c'),
    'MessageWindow_Clear': ('int', 'src/fn/sub_08030BF8.c'),
    'SceneMessages_UnregisterReceiver': ('s32', 'src/fn/sub_08020D08.c'),
    'SceneMessages_SetPendingTrigger': ('u32', 'src/fn/sub_08020D54.c'),
}

DECLARATION = re.compile(r'(?m)^([ \t]*(?:extern[ \t]+)?)void([ \t]+)(' +
                         '|'.join(map(re.escape, RETURNS)) + r')(?=[ \t]*\()')


def verify_returns(root):
    """Fail if an integration changes the definition underlying a correction."""
    for name, (result, source) in RETURNS.items():
        pattern = (r'^' + re.escape(result) + r'\s+' + re.escape(name) +
                   r'\s*\([^;{}]*\)\s*\{')
        path = root / source
        if path.exists() and re.search(pattern, path.read_text(), re.M):
            continue
        # Units move as the decomp is reorganized; the definition is the evidence.
        if not any(re.search(pattern, p.read_text(), re.M)
                   for p in root.glob('src/**/*.c') if name in p.read_text()):
            raise RuntimeError('Port return-type evidence changed: ' + name + ' in ' + source)


def repair_returns(text):
    return DECLARATION.sub(lambda m: m[1] + RETURNS[m[3]][0] + m[2] + m[3], text)
