"""Small, audited source adaptations for clang, confined to generated build files."""
import re
from pathlib import Path


def prepare_source(path, target):
    text = path.read_text()
    # The compatibility macro erases matching hints. Keep MMIO volatile intact.
    text = re.sub(r'\basm\s+(?:volatile|__volatile__)\s*\(', 'asm(', text)
    if path.name == 'actor_animation_controller.c':
        text = 'void *ScriptProc_Start(unsigned int, unsigned int);\n' + text
    if path.name == 'golem_2.c':
        text = ('struct SceneActorAnimationView;\n'
                'void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *, int);\n' + text)
    if path.name == 'sub_082486B4.c':
        text = text.replace('(u8 *)ptr += (gEEPROMConfig->address_width << 1) + 1;',
                            'ptr = (u16 *)((u8 *)ptr + (gEEPROMConfig->address_width << 1) + 1);')
        text = text.replace('((u8 *)ptr)++;', 'ptr = (u16 *)((u8 *)ptr + 1);')
    return text


def prepare_headers(root, output, target):
    """Mirror headers so nested quoted includes use the same portable versions."""
    for path in sorted((root / 'include').rglob('*.h')):
        text = path.read_text()
        if path.name == 'player_cue.h':
            # agbcc rounds all record sizes to a word. LLVM uses member alignment.
            text = text.replace('struct PlayerCueTimers {',
                                'struct __attribute__((aligned(4))) PlayerCueTimers {')
        if path.as_posix().endswith('gba/syscall.h'):
            text = text.replace('#define SystemCall(n) asm("swi " #n)',
                                '#define SystemCall(n) port_system_call(n)')
        if target == 'host':
            # Host objects are for selected subsystem tests, not GBA ABI execution.
            text = re.sub(r'typedef char \w+(?:SizeCheck|LayoutCheck)\[[^;]+;', '', text)
        destination = output / path.relative_to(root / 'include')
        destination.parent.mkdir(parents=True, exist_ok=True)
        if not destination.exists() or destination.read_text() != text:
            destination.write_text(text)
