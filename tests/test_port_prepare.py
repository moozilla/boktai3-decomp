import importlib
from pathlib import Path
import sys
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
with patch.object(sys, 'path', [str(ROOT / 'port/core'), *sys.path]):
    prepare = importlib.import_module('prepare')
    from source_layout import new_filename, symbol_names


class PrepareSourceTests(unittest.TestCase):
    def test_function_adaptations_follow_symbol_basenames(self):
        sources = (
            (0x08224980, 'void sub_08249240(u32, u32, u32);\n', 'int sub_08249240('),
            (0x08228C40, 'u32 Clock_PackDate(void);\nu32 f() { return Clock_PackDate(); }\n',
             'Clock_PackDate(a, b, c)'),
            (0x082290F4, 'void SunClock_SetDateTime(s32, s32, s32, s32, s32, s32, s32);\n',
             's32 SunClock_SetDateTime('),
            (0x08229380, 'void SunClock_SetDateTime(u32, u32, u32, u32, u32, u32, u32);\n',
             's32 SunClock_SetDateTime('),
            (0x082486B4, 'void f(u16 *ptr) { ((u8 *)ptr)++; }\n',
             'ptr = (u16 *)((u8 *)ptr + 1);'),
        )
        names = symbol_names(ROOT)
        for address, content, adaptation in sources:
            symbol = names[address]
            with self.subTest(symbol=symbol), patch.object(Path, 'read_text', return_value=content):
                old = ROOT / 'src' / f'sub_{address:08X}.c'
                named = old.with_name(new_filename(symbol, address))
                prepared = prepare.prepare_source(old, 'wasm32')
                self.assertIn(adaptation, prepared)
                self.assertEqual(prepare.prepare_source(named, 'wasm32'), prepared)
