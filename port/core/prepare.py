"""Small, audited source adaptations for clang, confined to generated build files."""
import re
from pathlib import Path


def prepare_source(path, target):
    text = path.read_text()
    text = text.replace('asm("swi 0x2A")', 'port_system_call(0x2a)')
    # The compatibility macro erases matching hints. Keep MMIO volatile intact.
    text = re.sub(r'\basm\s+(?:volatile|__volatile__)\s*\(', 'asm(', text)
    if path.name == 'actor_animation_controller.c':
        text = 'void *ScriptProc_Start(unsigned int, unsigned int);\n' + text
    if path.name == 'golem_2.c':
        text = ('struct SceneActorAnimationView;\n'
                'void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *, int);\n' + text)
    if path.name == 'sub_08213D10.c':
        # The dispatcher copy is data; a wasm function pointer is not ROM bytes.
        text = text.replace('(u32)IntrMain', '(u32)0x080001f0')
    if path.name == 'sub_082486B4.c':
        text = text.replace('(u8 *)ptr += (gEEPROMConfig->address_width << 1) + 1;',
                            'ptr = (u16 *)((u8 *)ptr + (gEEPROMConfig->address_width << 1) + 1);')
        text = text.replace('((u8 *)ptr)++;', 'ptr = (u16 *)((u8 *)ptr + 1);')
    if path.name == 'libgcc_dp.c':
        # ARM APCS doubles put the high word first. wasm/host IEEE doubles do not.
        text = text.replace('  {\n    halffractype tmp = dst.words[0];\n    dst.words[0] = dst.words[1];\n    dst.words[1] = tmp;\n  }', '')
        text = text.replace('  FLO_union_type swapped;\n  swapped.words[0] = src->words[1];\n  swapped.words[1] = src->words[0];\n  src = &swapped;', '')
    if target == 'host':
        text = host_layout(text)
    return text


def prepare_headers(root, output, target):
    """Mirror headers so nested quoted includes use the same portable versions."""
    for path in sorted((root / 'include').rglob('*.h')):
        text = path.read_text()
        if path.name == 'player_cue.h':
            # agbcc rounds all record sizes to a word. LLVM uses member alignment.
            text = text.replace('struct PlayerCueTimers {',
                                'struct __attribute__((aligned(4))) PlayerCueTimers {')
        if path.name == 'libm_compat.h':
            text = text.replace('msw, lsw;', 'lsw, msw;')
        if path.as_posix().endswith('gba/syscall.h'):
            text = text.replace('#define SystemCall(n) asm("swi " #n)',
                                '#define SystemCall(n) port_system_call(n)')
        if target == 'host':
            # Host objects are for selected subsystem tests, not GBA ABI execution.
            text = host_layout(text)
        destination = output / path.relative_to(root / 'include')
        destination.parent.mkdir(parents=True, exist_ok=True)
        if not destination.exists() or destination.read_text() != text:
            destination.write_text(text)


def host_layout(text):
    # Host pointers are wider. Keep record members but omit GBA-ABI assertions.
    text = re.sub(r'typedef\s+char\s+\w+\s*\[(?=[^;]*\?\s*1\s*:\s*-1)[^;]+;', '', text)
    # Three explicit padding gaps disappear when host pointers grow past them.
    for field in ('unknown678', 'unknown6CC', 'pad'):
        text = re.sub(r'\b' + field + r'\[(0x[0-9A-Fa-f]+\s*-\s*0x[0-9A-Fa-f]+\s*-\s*sizeof\([^)]*\))\]',
                      lambda m: field + '[(' + m[1] + ') < 0x10000000u ? (' + m[1] + ') : 0]', text)
    return text
