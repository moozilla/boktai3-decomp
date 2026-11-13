"""Small, audited source adaptations for clang, confined to generated build files."""
import re
from pathlib import Path
from returns import repair_returns, verify_returns

ROOT_SRC = Path(__file__).resolve().parents[2] / 'src'


def prepare_source(path, target):
    text = repair_returns(path.read_text())
    text = text.replace('asm("swi 0x2A")', 'port_system_call(0x2a)')
    if path.name == 'devices.c' and path.parent.name == 'hw':
        # Leave callers at the public boundary so core can observe every IO
        # publication, including identical-value writes performed by DMA.
        before = 'void hw_publish_io(Hw *hw, unsigned offset, unsigned size)'
        if text.count(before) != 1:
            raise RuntimeError('port-hw IO publication boundary changed')
        text = text.replace(before, 'void port_hw_publish_io(Hw *hw, unsigned offset, unsigned size)')
    # The compatibility macro erases matching hints. Keep MMIO volatile intact.
    text = re.sub(r'\basm\s+(?:volatile|__volatile__)\s*\(', 'asm(', text)
    if path.name == 'actor_animation_controller.c':
        text = 'void *ScriptProc_Start(unsigned int, unsigned int);\n' + text
    if path.name == 'script_virtual_machine.c':
        # ScriptProc_Start's result stays in r0 as Script_LoadScript's argument.
        text = text.replace('    ScriptProc_Start(0xA8D9, 0x49BC);\n', '')
        text = text.replace('((s32(*)())Script_LoadScript)();',
                            '((s32(*)())Script_LoadScript)(ScriptProc_Start(0xA8D9, 0x49BC));')
    if path.name == 'ghost.c':
        # Called before its prototype; agbcc accepts the implicit int, clang does not.
        text = 'void Anim_SetDurationScale(void *, unsigned int);\n' + text
    if path.name == 'golem_2.c':
        text = ('struct SceneActorAnimationView;\n'
                'void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *, int);\n' + text)
    if path.name == 'sub_08224980.c':
        # The portable SDK call-via bridge returns a word; this caller ignores it.
        text = text.replace('void sub_08249240(', 'int sub_08249240(')
    if path.name == 'sub_08228C40.c':
        # The GBA call carries the incoming date in r0/r1/r2 unchanged.
        # wasm needs explicit arguments; see the original 08228C40 prologue.
        text = text.replace('u32 Clock_PackDate(void);', 'u32 Clock_PackDate(s32, s32, s32);')
        text = text.replace('Clock_PackDate();', 'Clock_PackDate(a, b, c);')
    if path.name in ('sub_082290F4.c', 'sub_08229380.c'):
        # These callers discard the result, but wasm still checks the type.
        text = text.replace('void SunClock_SetDateTime(', 's32 SunClock_SetDateTime(')
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
    elif path.is_relative_to(ROOT_SRC):
        text = agbcc_records(text)
    return text


def prepare_headers(root, output, target):
    """Mirror headers so nested quoted includes use the same portable versions."""
    verify_returns(root)
    for path in sorted((root / 'include').rglob('*.h')):
        text = repair_returns(path.read_text())
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
        else:
            text = agbcc_records(text)
        destination = output / path.relative_to(root / 'include')
        destination.parent.mkdir(parents=True, exist_ok=True)
        if not destination.exists() or destination.read_text() != text:
            destination.write_text(text)


_RECORD = re.compile(r'\b(struct|union)(\s+__attribute__\s*\(\([^)]*\)\))?(\s+\w+)?\s*\{')


def agbcc_records(text):
    """agbcc (APCS) aligns every struct and union to 4 bytes and rounds its size
    to a multiple of 4: struct { u8 a, b; } is 4 bytes, alignment 4. Records the
    source marks packed keep their explicit layout."""
    out, pos = [], 0
    for m in _RECORD.finditer(text):
        if m.start() < pos:
            continue
        depth, i = 0, m.end() - 1
        while i < len(text):
            if text[i] == '{':
                depth += 1
            elif text[i] == '}':
                depth -= 1
                if depth == 0:
                    break
            i += 1
        tail = text[i:i + 200].split(';', 1)[0]
        if 'packed' in tail or (m[2] and 'packed' in m[2]):
            continue
        out.append(text[pos:m.start()])
        out.append(m[1] + ' __attribute__((aligned(4)))' + text[m.start() + len(m[1]):m.end()])
        pos = m.end()
    out.append(text[pos:])
    return ''.join(out)


def host_layout(text):
    # Host pointers are wider. Keep record members but omit GBA-ABI assertions.
    text = re.sub(r'typedef\s+char\s+\w+\s*\[(?=[^;]*\?\s*1\s*:\s*-1)[^;]+;', '', text)
    # Three explicit padding gaps disappear when host pointers grow past them.
    for field in ('unknown678', 'unknown6CC', 'pad'):
        text = re.sub(r'\b' + field + r'\[(0x[0-9A-Fa-f]+\s*-\s*0x[0-9A-Fa-f]+\s*-\s*sizeof\([^)]*\))\]',
                      lambda m: field + '[(' + m[1] + ') < 0x10000000u ? (' + m[1] + ') : 0]', text)
    return text
