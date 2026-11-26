"""Correct discarded-return declarations using verified existing C definitions.

Only explicit void declarations are changed. Calls already discard the result,
so this does not invent arguments, values, or behavior. Parameter reconciliation
and indirect callback types remain separate tasks.
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
from source_layout import locate

# Function and its verified return type.
RETURNS = {
    'Task_Kill': 's32',
    'Script_Call': 's32',
    'Trigger_ConsumePending': 'u32',
    'AnimSprite_InitLinked': 's32',
    'Actor_PlayAnimationEntry': 'int',
    'AnimData_Load': 's32',
    'Anim_SetFrameDurationScale': 's32',
    'DamageNumberDisplay_Show': 's32',
    'Anim_RestartSequence': 'u32',
    'SparkParticle_Spawn': 's32',
    'Anim_LoadKeyframe': 's32',
    'TextWindow_StartPlayback': 's32',
    'MenuDisplay_UpdateActionPrompt': 's32',
    'Anim_SetSequence': 'u32',
    'ScrollingDialogue_ClearScript': 'u32',
    'ResourceHeader_Relocate': 's32',
    'TextWindow_Refresh': 's32',
    'SceneBody_Unregister': 's32',
    'Anim_Step': 's32',
    'SceneBody_Init': 'u32',
    'MotionTrack_Update': 's32',
    'Shadow_Destroy': 'u32',
    'MessageWindow_Clear': 'int',
    'SceneMessages_UnregisterReceiver': 's32',
    'SceneMessages_SetPendingTrigger': 'u32',
}

DECLARATION = re.compile(r'(?m)^([ \t]*(?:extern[ \t]+)?)void([ \t]+)(' +
                         '|'.join(map(re.escape, RETURNS)) + r')(?=[ \t]*\()')


def verify_returns(root):
    """Fail if an integration changes the definition underlying a correction."""
    for name, result in RETURNS.items():
        pattern = (r'^' + re.escape(result) + r'\s+' + re.escape(name) +
                   r'\s*\([^;{}]*\)\s*\{')
        entry = locate(name, root)
        if entry and re.search(pattern, (root / entry[0]).read_text(), re.M):
            continue
        raise RuntimeError('Port return-type evidence changed: ' + name)


def repair_returns(text):
    return DECLARATION.sub(lambda m: m[1] + RETURNS[m[3]] + m[2] + m[3], text)
