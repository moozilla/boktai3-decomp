"""Player variant tables preserve physical slots, numeric bases and ROM links."""
import struct
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
for directory in (ROOT / 'tools', ROOT / 'tools' / 'assets', ROOT / 'tools' / 'studio'):
    sys.path.insert(0, str(directory))

import actors
import atlas
import datafmt
import player_cue
from datafmt import player_animations as codec


class PlayerAnimationCodecTests(unittest.TestCase):
    def setUp(self):
        self.start = codec.ROM_BASE + 4
        self.end = self.start + codec.SIZE
        self.values = list(range(len(codec.MODES) * codec.SLOTS))
        self.values[35] = 0xFFFF
        self.raw = struct.pack(f'<{len(self.values)}H', *self.values)
        self.text = codec.decode(b'head' + self.raw + b'tail', self.start, self.end, {})

    def test_roundtrip_retains_sentinel_and_emits_halfwords_without_relocations(self):
        items = codec.encode(self.text, self.start, self.end)
        self.assertEqual(datafmt.to_bytes(items), self.raw)
        self.assertTrue(all(size == 2 and pointer is False for size, _, pointer in items))
        rows = codec.parse(self.text, self.start, self.end)
        self.assertEqual(rows[35]['base_sequence'], 0xFFFF)
        self.assertEqual(rows[3 * codec.SLOTS]['mode'], 6)

    def test_rows_cannot_be_reordered_duplicated_or_resized(self):
        lines = self.text.splitlines()
        for changed in (lines[:1] + lines[2:3] + lines[1:2] + lines[3:],
                        lines[:-1], lines + [lines[-1]],
                        [line.replace('6,0,', '3,0,') for line in lines]):
            with self.subTest(rows=changed[:3]), self.assertRaises(ValueError):
                codec.encode('\n'.join(changed), self.start, self.end)

    def test_values_columns_and_extents_are_validated(self):
        for value in ('-1', '10000', 'not-a-sequence'):
            with self.subTest(value=value), self.assertRaises(ValueError):
                codec.encode(self.text.replace('0,0,0000', '0,0,' + value), self.start, self.end)
        with self.assertRaises(ValueError):
            codec.encode('mode,index\n', self.start, self.end)
        for start, end in ((self.start + 1, self.end + 1), (self.start, self.end - 2)):
            with self.subTest(start=start, end=end), self.assertRaises(ValueError):
                codec.encode(self.text, start, end)
        with self.assertRaises(ValueError):
            codec.decode(b'short', self.start, self.end, {})


class PlayerAnimationStudioTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.start = 0x08610980
        self.end = self.start + codec.SIZE
        self.relative = 'data/player_cue/animations.csv'
        path = self.root / self.relative
        path.parent.mkdir(parents=True)
        self.values = [0xFFFF] * (len(codec.MODES) * codec.SLOTS)
        self.values[0] = 3
        raw = struct.pack(f'<{len(self.values)}H', *self.values)
        path.write_text(codec.decode(bytes(self.start - codec.ROM_BASE) + raw,
                                     self.start, self.end, {}))
        entry = datafmt.Entry({'name': 'variants', 'start': f'{self.start:08X}',
                               'end': f'{self.end:08X}', 'format': 'player_animations',
                               'file': self.relative, 'description': 'Variants'})
        entry.file = str(path)
        self.contexts = [patch.object(player_cue, 'ROOT', str(self.root)),
                         patch.object(player_cue, 'load_manifest', return_value=[entry])]
        funcs = {0x0814FC80: {}, 0x0814F804: {}, 0x0813AE18: {},
                 0x0815B000: {}, 0x0815E034: {}}
        names = {'OldLookup': player_cue.LOOKUP}
        calls = {addr: ['OldLookup'] for addr in funcs}
        calls[0x0814F804] = ['NearbyFunction']
        self.contexts.extend([patch.object(actors, 'code', return_value=(funcs, names)),
                              patch.object(actors, 'calls', side_effect=calls.__getitem__)])
        for context in self.contexts:
            context.start()
            self.addCleanup(context.stop)
        symbols = self.root / 'symbols/functions.csv'
        symbols.parent.mkdir()
        symbols.write_text('addr,name\n081419D4,CurrentLookup\n0813AE18,CurrentSelect\n'
                           '0814FC80,CurrentCue\n0815B000,CurrentLaterCue\n')

    def test_data_and_function_names_follow_sources_and_direct_calls(self):
        data = player_cue.animation_variants()
        self.assertEqual(data['source'], self.relative)
        self.assertEqual(data['size'], 464)
        self.assertEqual(data['lookup'], {'addr': '081419D4', 'name': 'CurrentLookup'})
        self.assertEqual(data['tables'][0]['entries'][0]['base_sequence'], 3)
        self.assertEqual(data['consumers'], [
            {'addr': '0814FC80', 'name': 'CurrentCue'},
            {'addr': '0815B000', 'name': 'CurrentLaterCue'}])
        path = self.root / self.relative
        path.write_text(path.read_text().replace('0,0,0003', '0,0,0007'))
        self.assertEqual(player_cue.animation_variants()['tables'][0]['entries'][0]['base_sequence'], 7)

    def test_rom_blocks_cover_only_four_tables(self):
        blocks = atlas.player_animation_blocks()
        self.assertEqual([(block['s'], block['n']) for block in blocks],
                         [(self.start + i * 116, 116) for i in range(4)])
        self.assertEqual(sum(block['n'] for block in blocks), 464)
        self.assertTrue(all(block['u'] == 'decoded' and block['r'] == '/actors#player-animations'
                            for block in blocks))


if __name__ == '__main__':
    unittest.main()
