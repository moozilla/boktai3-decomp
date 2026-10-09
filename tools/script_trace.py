#!/usr/bin/env python3
"""Join U33J script/actor probes into checked nested native-call records.

Input is the harness probes.tsv from the named probe sites in
 tests/script_probes.txt. This reports observed calls, not inferred semantics.
ROM-derived JSON output must remain under ignored build/.
"""
import argparse
import csv
import json
import struct
from pathlib import Path

SITES = {
    'native_enter': 0x0821AC6C, 'native_lookup': 0x0821AC7C,
    'native_call': 0x0821AC9E, 'native_return': 0x0821ACA2,
    'actor_key': 0x0822556A, 'actor_resolved': 0x0822556E, 'actor_call': 0x0822557E,
    'actor_return': 0x08225582,
}


def actor_registry(rom):
    """Read the independently audited U33J registry, including its sentinel."""
    begin, end = 0x603300, 0x604990
    if len(rom) < end + 8:
        raise ValueError('ROM is too short for the U33J actor registry')
    entries = list(struct.iter_unpack('<II', rom[begin:end]))
    if any(not target & 1 for _, target in entries):
        raise ValueError('actor registry contains a non-Thumb callback')
    if any(a[0] >= b[0] for a, b in zip(entries, entries[1:])):
        raise ValueError('actor registry keys are not strictly increasing')
    if int.from_bytes(rom[end+4:end+8], 'little') != 0:
        raise ValueError('actor registry lacks the expected null sentinel')
    return dict(entries)


def join_events(rows, rom=None):
    stack, calls = [], []
    registry = None
    previous_sequence = 0
    for row in rows:
        sequence = int(row['sequence'])
        if sequence <= previous_sequence:
            raise ValueError('probe sequence must increase strictly')
        previous_sequence = sequence
        kind = row['probe']
        if kind not in SITES:
            continue
        pc = int(row['pc'], 16)
        if pc != SITES[kind] or row['mode'] != 'thumb':
            raise ValueError(f'{kind}: unexpected probe address or execution mode')
        r = [int(row[f'r{i}'], 16) for i in range(16)]
        frame = int(row['frame'])
        if kind == 'native_enter':
            call = {'sequence': sequence, 'frame': frame, 'segment': row['segment'],
                    'depth': len(stack), 'script_pointer': r[0], 'entry_sp': r[13],
                    'actors': [], 'stage': 'entered'}
            calls.append(call)
            stack.append(call)
            continue
        if not stack:
            raise ValueError(f'{kind}: missing native_enter (trace began mid-call or lost events)')
        call = stack[-1]
        if kind.startswith('native_') and r[13] != call['entry_sp'] - 16:
            raise ValueError(f'{kind}: native stack pointer does not match its entry')
        if kind == 'native_lookup':
            if call['stage'] != 'entered':
                raise ValueError('duplicate or out-of-order native lookup')
            call['command_id'] = r[0]
            offset = call['script_pointer'] - 0x08000000
            if rom is not None and 0 <= offset < len(rom):
                if offset + 2 > len(rom) or int.from_bytes(rom[offset:offset+2], 'little') != r[0]:
                    raise ValueError('native command ID disagrees with ROM script bytes')
                call['command_id_verified_in_rom'] = True
            call['stage'] = 'looked_up'
        elif kind == 'native_call':
            if call['stage'] != 'looked_up' or not r[1] & 1:
                raise ValueError('native call is out of order or target lacks Thumb bit')
            call.update(stage='called', argument_pointer=r[0], native_target=r[1] & ~1,
                        table_entry=r[5])
            if rom is not None:
                offset = r[5] - 0x08000000
                if 0 <= offset and offset + 8 <= len(rom):
                    identifier = int.from_bytes(rom[offset:offset+4], 'little')
                    target = int.from_bytes(rom[offset+4:offset+8], 'little')
                    if (identifier, target) != (call['command_id'], r[1]):
                        raise ValueError('resolved command table entry disagrees with ROM')
                    call['table_entry_verified_in_rom'] = True
        elif kind == 'native_return':
            if call['stage'] != 'called' or any(a['stage'] not in ('returned', 'missing') for a in call['actors']):
                raise ValueError('native return precedes call or has an unfinished actor call')
            call.update(stage='returned', return_frame=frame, return_r0=r[0])
            stack.pop()
        else:
            if call.get('native_target') != 0x08225560 or call['stage'] != 'called':
                raise ValueError('actor probe is outside the verified chara native handler')
            if kind == 'actor_key':
                if call['actors'] and call['actors'][-1]['stage'] not in ('returned', 'missing'):
                    raise ValueError('actor key overwrites an unfinished actor call')
                call['actors'].append({'registry_key': r[0], 'frame': frame,
                                       'stage': 'key', 'sp': r[13]})
            else:
                if not call['actors']:
                    raise ValueError('actor call/return is missing its lookup key')
                actor = call['actors'][-1]
                if actor['sp'] != r[13]:
                    raise ValueError('actor stack pointer changed across the callback')
                if kind == 'actor_resolved':
                    if actor['stage'] != 'key' or (r[0] and not r[0] & 1):
                        raise ValueError('invalid actor lookup result')
                    if rom is not None:
                        if registry is None:
                            registry = actor_registry(rom)
                        if registry.get(actor['registry_key'], 0) != r[0]:
                            raise ValueError('resolved actor callback disagrees with ROM registry')
                        actor['target_verified_in_rom'] = True
                    actor.update(stage='resolved' if r[0] else 'missing', target=r[0] & ~1)
                elif kind == 'actor_call':
                    if actor['stage'] != 'resolved' or not r[4] & 1 or r[4] & ~1 != actor['target']:
                        raise ValueError('invalid actor callback order or Thumb target')
                    actor.update(stage='called', target=r[4] & ~1, argument_r0=r[0], argument_r1=r[1])
                else:
                    if actor['stage'] != 'called':
                        raise ValueError('actor return precedes callback')
                    actor.update(stage='returned', return_r0=r[0])
    if stack:
        raise ValueError(f'trace ends with {len(stack)} unfinished native calls')
    return calls


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('trace', type=Path)
    parser.add_argument('--rom', type=Path, help='verify ROM-backed script IDs, command entries and actor registry results')
    args = parser.parse_args()
    try:
        with args.trace.open(newline='') as f:
            calls = join_events(csv.DictReader(f, delimiter='\t', quoting=csv.QUOTE_NONE),
                                args.rom.read_bytes() if args.rom else None)
    except (ValueError, KeyError, OSError) as error:
        parser.exit(2, f'error: {error}\n')
    print(json.dumps({'native_calls': len(calls),
                      'actor_lookups': sum(len(c['actors']) for c in calls),
                      'actor_calls': sum(a['stage'] == 'returned' for c in calls for a in c['actors']),
                      'calls': calls}, indent=2))


if __name__ == '__main__':
    main()
