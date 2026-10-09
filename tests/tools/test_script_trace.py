"""Synthetic probe sequences exercise nesting, omissions and null actor lookup."""
import importlib.util
import struct
from pathlib import Path
import unittest

spec = importlib.util.spec_from_file_location('script_trace', Path(__file__).parents[2] / 'tools/script_trace.py')
trace = importlib.util.module_from_spec(spec)
spec.loader.exec_module(trace)


def event(kind, sp=0x3007000, **regs):
    row = dict(sequence='0', frame='7', segment='fixture', probe=kind,
               pc=f'{trace.SITES[kind]:08X}', mode='thumb')
    row.update({f'r{i}': '00000000' for i in range(16)})
    row['r13'] = f'{sp:X}'
    row.update({k: f'{v:X}' for k, v in regs.items()})
    return row


def call_prefix(sp=0x3007000, target=0x821B795):
    return [event('native_enter', sp, r0=0x8000004),
            event('native_lookup', sp-16, r0=0xD86),
            event('native_call', sp-16, r0=0x8000007, r1=target, r5=0x8000010)]


def numbered(rows):
    for i, row in enumerate(rows, 1): row['sequence'] = str(i)
    return rows


class TraceTests(unittest.TestCase):
    def test_nested_calls_and_rom_evidence(self):
        rows = call_prefix() + call_prefix(0x3006F00)
        rows += [event('native_return', 0x3006EF0), event('native_return', 0x3006FF0)]
        rom = bytearray(32)
        rom[4:6] = (0xD86).to_bytes(2, 'little')
        rom[16:20] = (0xD86).to_bytes(4, 'little')
        rom[20:24] = (0x821B795).to_bytes(4, 'little')
        calls = trace.join_events(numbered(rows), rom)
        self.assertEqual([c['depth'] for c in calls], [0, 1])
        self.assertTrue(all(c['table_entry_verified_in_rom'] for c in calls))
        rom[4] ^= 1
        with self.assertRaisesRegex(ValueError, 'disagrees with ROM'):
            trace.join_events(rows, rom)

    def test_actor_success_and_missing_lookup(self):
        for target in (0, 0x8012345):
            rows = call_prefix(target=0x8225561)
            rows += [event('actor_key', 0x3006FE8, r0=123),
                     event('actor_resolved', 0x3006FE8, r0=target)]
            if target:
                rows += [event('actor_call', 0x3006FE8, r4=target, r0=456),
                         event('actor_return', 0x3006FE8, r0=789)]
            rows += [event('native_return', 0x3006FF0)]
            actor = trace.join_events(numbered(rows))[0]['actors'][0]
            self.assertEqual(actor['stage'], 'returned' if target else 'missing')
            self.assertEqual(actor['registry_key'], 123)
            if target: self.assertEqual(actor['argument_r0'], 456)

    def test_incomplete_wrong_site_or_stack_cannot_look_complete(self):
        with self.assertRaisesRegex(ValueError, 'unfinished'):
            trace.join_events(numbered(call_prefix()))
        rows = numbered(call_prefix() + [event('native_return', 0x3006FF4)])
        with self.assertRaisesRegex(ValueError, 'stack pointer'):
            trace.join_events(rows)
        rows[0]['pc'] = '0821AC6E'
        with self.assertRaisesRegex(ValueError, 'unexpected probe address'):
            trace.join_events(rows)
        with self.assertRaisesRegex(ValueError, 'missing native_enter'):
            trace.join_events(numbered([event('native_return')]))

    def test_duplicate_or_missing_events_fail(self):
        rows = call_prefix()
        rows.insert(2, event('native_lookup', 0x3006FF0, r0=0xD86))
        with self.assertRaisesRegex(ValueError, 'duplicate or out-of-order'):
            trace.join_events(numbered(rows))
        rows = numbered(call_prefix())
        rows[1]['sequence'] = rows[0]['sequence']
        with self.assertRaisesRegex(ValueError, 'increase strictly'):
            trace.join_events(rows)

    def test_actor_registry_evidence_rejects_wrong_target(self):
        rom = bytearray(0x604998)
        for i in range(722):
            struct.pack_into('<II', rom, 0x603300 + i * 8, i + 1, 0x8012345 + i * 2)
        rom[4:6] = (0xD86).to_bytes(2, 'little')
        struct.pack_into('<II', rom, 16, 0xD86, 0x8225561)
        rows = call_prefix(target=0x8225561)
        rows += [event('actor_key', 0x3006FE8, r0=1),
                 event('actor_resolved', 0x3006FE8, r0=0x8012345),
                 event('actor_call', 0x3006FE8, r4=0x8012345),
                 event('actor_return', 0x3006FE8),
                 event('native_return', 0x3006FF0)]
        calls = trace.join_events(numbered(rows), rom)
        self.assertTrue(calls[0]['actors'][0]['target_verified_in_rom'])
        struct.pack_into('<I', rom, 0x603304, 0x8012347)
        with self.assertRaisesRegex(ValueError, 'disagrees with ROM registry'):
            trace.join_events(rows, rom)
        with self.assertRaisesRegex(ValueError, 'too short'):
            trace.actor_registry(bytes(16))


if __name__ == '__main__':
    unittest.main()
