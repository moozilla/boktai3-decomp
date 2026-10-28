"""Regression tests for ROM-free progress counts and objdiff v2 JSON types."""
import json
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
import progress


class ProgressTests(unittest.TestCase):
    def test_library_groups_preserve_function_and_byte_credit(self):
        funcs = [('before', 0x08000100), ('one', 0x08000104),
                 ('two', 0x08000108), ('after', 0x08000110)]
        sizes = dict(before=4, one=4, two=8, after=4)
        done = {'before', 'one', 'after'}
        libraries = [{'name': 'SDK', 'start': 0x08000104, 'end': 0x08000110,
                      'names': {0x08000104: 'VerifiedName'}}]
        report = progress.make_report(funcs, sizes, done, libraries)
        old = progress.make_report(funcs, sizes, done)
        for key in ('total_code', 'matched_code', 'complete_code', 'total_functions',
                    'matched_functions', 'fuzzy_match_percent'):
            self.assertEqual(report['measures'][key], old['measures'][key])
        self.assertEqual(report['measures']['total_units'], 3)
        self.assertEqual(report['measures']['complete_units'], 2)
        unit = report['units'][1]
        self.assertEqual(unit['name'], 'libraries/SDK')
        self.assertEqual([f['name'] for f in unit['functions']], ['VerifiedName', 'two'])
        self.assertEqual([f['address'] for f in unit['functions']], ['0', '4'])
        self.assertEqual(unit['measures']['matched_code'], '4')
        self.assertEqual(unit['measures']['total_code'], '12')
        self.assertFalse(unit['metadata']['complete'])
        complete = progress.make_report(funcs, sizes, set(sizes), libraries)
        self.assertEqual(complete['measures']['complete_units'], 3)
        self.assertTrue(complete['units'][1]['metadata']['complete'])

    def test_reject_overlapping_and_unaligned_library_bounds(self):
        funcs = [('a', 0x08000100), ('b', 0x08000104), ('c', 0x08000108)]
        sizes = dict(a=4, b=4, c=4)
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'groups.csv'
            header = 'name,start,end,name_files,evidence\n'
            for rows in ('SDK,08000100,08000106,,reviewed\n',
                         'SDK,08000100,08000108,,reviewed\n'
                         'Other,08000104,08000108,,reviewed\n'):
                path.write_text(header + rows)
                with self.assertRaises(ValueError):
                    progress.load_libraries(funcs, sizes, path)

    def test_repository_library_spans_and_names(self):
        funcs, sizes = progress.load_functions()
        libraries = progress.load_libraries(funcs, sizes)
        self.assertEqual(len(libraries), 8)
        report = progress.make_report(funcs, sizes, set(), libraries)
        nested = [f for u in report['units'] for f in u['functions']]
        self.assertEqual([int(f['metadata']['virtual_address']) for f in nested],
                         [a for _, a in funcs])
        self.assertEqual(sum(int(f['size']) for f in nested), sum(sizes.values()))
        sdk_names = {f['name'] for u in report['units']
                     if u['name'].startswith('libraries/') for f in u['functions']}
        self.assertTrue({'__adddf3', '__kernel_sin', 'STWI_init_all',
                         'SiiRtcSetTime', 'EEPROMRead', 'm4aSoundMode', 'memcpy'} <= sdk_names)

    def test_subsystem_groups_preserve_credit_and_pinning(self):
        funcs, sizes = progress.load_functions()
        done = progress.collect_done(funcs, Path(progress.ROOT) / "src")
        pinned = progress.collect_pinned(funcs, Path(progress.ROOT) / "src") & done
        libraries = progress.load_libraries(funcs, sizes)
        groups = progress.load_groups(funcs, sizes, progress.SUBSYSTEMS, "game")
        before = progress.make_report(funcs, sizes, done, libraries, pinned)
        after = progress.make_report(funcs, sizes, done, libraries, pinned, groups)
        for key in ("total_code", "matched_code", "matched_functions"):
            self.assertEqual(before["measures"][key], after["measures"][key])
        original = {f["metadata"]["virtual_address"]: f["fuzzy_match_percent"]
                    for u in before["units"] for f in u["functions"]}
        grouped = [f for u in after["units"] for f in u["functions"]]
        self.assertEqual(len(grouped), len(original))
        self.assertEqual(original, {f["metadata"]["virtual_address"]:
                                   f["fuzzy_match_percent"] for f in grouped})
        self.assertTrue(any(u["name"] == "game/Task scheduler" for u in after["units"]))
        for a, b in zip(before["categories"], after["categories"]):
            self.assertEqual(a["measures"]["matched_code"], b["measures"]["matched_code"])
        with self.assertRaisesRegex(ValueError, "overlap"):
            progress.make_report(funcs, sizes, done, libraries, pinned, groups + groups)

    def test_scalar_helper_does_not_absorb_retained_math_routines(self):
        funcs, sizes = progress.load_functions()
        names = {address: name for name, address in funcs}
        # The old next-labelled-start span credited 2,484 retained ASM bytes
        # to this independently verified 52-byte C helper.
        self.assertEqual(sizes[names[0x08221800]], 52)
        self.assertEqual(sizes[names[0x08221834]], 40)
        tail = [name for name, address in funcs
                if 0x08221834 <= address < 0x082221E8]
        self.assertEqual(len(tail), 31)
        self.assertEqual(sum(sizes[name] for name in tail), 2484)

    def test_inventory_and_named_address_alias(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            inventory = root / 'inventory.csv'
            progress.write_inventory(inventory, [progress.CODE_END - 12, progress.CODE_END - 8])
            symbols = root / 'functions.csv'
            symbols.write_text('addr,name\n0824DAEE,NamedFunction\n')
            funcs, sizes = progress.load_functions(inventory, symbols)
            (root / 'one.c').write_text('void sub_0824DAEE(void) { }\n'
                                      'static void helper(void) { }\n'
                                      'INCLUDE_ASM("asm/nonmatching", sub_0824DAF2);\n')
            # Duplicate declarations in another file must not inflate counts.
            (root / 'two.c').write_text('void NamedFunction(void) { }\n')
            done = progress.collect_done(funcs, root)
            self.assertEqual(done, {'NamedFunction'})
            report = json.loads(json.dumps(progress.make_report(funcs, sizes, done)))
            self.assertEqual(report['version'], 2)
            self.assertEqual(report['measures']['total_code'], '12')
            self.assertEqual(report['measures']['matched_code'], '4')
            self.assertEqual(report['measures']['total_functions'], 2)
            self.assertEqual(report['measures']['matched_functions'], 1)
            self.assertEqual(report['measures']['total_units'], 2)
            self.assertEqual(report['measures']['complete_units'], 1)
            self.assertEqual(report['measures']['matched_functions_percent'], 50)
            self.assertEqual(sum(int(u['measures']['total_code']) for u in report['units']), 12)
            self.assertEqual(sum(int(u['measures']['matched_code']) for u in report['units']), 4)
            self.assertIsInstance(report['units'][0]['functions'][0]['size'], str)
            self.assertIsInstance(report['units'][0]['functions'][0]['metadata']['virtual_address'], str)

    def test_uncovered_tails_flag_short_matched_functions(self):
        funcs = [('a', 0x08000100), ('b', 0x08000110), ('c', 0x08000200)]
        sizes = dict(a=0x10, b=0xF0, c=4)
        code = {0x08000100: 0x0E, 0x08000110: 0x20, 0x08000200: 4}
        self.assertEqual(progress.uncovered_tails(funcs, sizes, {'a', 'b'}, code),
                         [('b', 0x08000110, 0xF0, 0x20)])
        self.assertEqual(progress.uncovered_tails(funcs, sizes, {'a'}, code), [])

    def test_reject_bad_inventory(self):
        for addresses in ([], [0x08000100, 0x08000100], [0x08000200, 0x08000100],
                          [0x07000000], [progress.CODE_END]):
            with self.subTest(addresses=addresses), self.assertRaises(ValueError):
                progress.validate_addresses(addresses)

    def test_reject_unknown_definition(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'unknown.c').write_text('void sub_08000104(void) { }\n')
            with self.assertRaisesRegex(ValueError, 'add_function'):
                progress.collect_done([('sub_08000100', 0x08000100)], root)

    def test_split_definition_gets_boundary(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            inventory = root / 'inventory.csv'
            progress.write_inventory(inventory, [0x08000100])
            symbols = root / 'functions.csv'
            symbols.write_text('addr,name\n08000108,TailFunction\n')
            (root / 'unit.c').write_text('void sub_08000100(void) { }\nvoid TailFunction(void) { }\n'
                                         'void sub_08000104(void) { }\nvoid NoAddress(void) { }\n')
            self.assertEqual(progress.source_boundaries(inventory, root, symbols),
                             [0x08000104, 0x08000108])


class AreaCategoryTest(unittest.TestCase):
    def test_areas_add_categories_without_changing_totals(self):
        areas = progress.load_areas()
        report = progress.make_report([("a", 0x08000000), ("b", 0x08086000)],
                                      {"a": 4, "b": 4}, {"b"}, areas=areas,
                                      categories=progress.load_categories())
        cats = {c["id"]: c["measures"] for c in report["categories"]}
        self.assertEqual(cats["intentional_asm"]["total_code"], "4")
        self.assertEqual(cats["monsters"]["matched_code"], "4")
        self.assertEqual(cats["decompilable"]["total_code"], "4")
        self.assertEqual(report["measures"]["total_code"], "8")
        self.assertIn("Monsters/08086000", [u["name"] for u in report["units"]])

    def test_markdown_subsystems_nest_families(self):
        unit = lambda name, done: {"name": name, "measures": {"total_code": "4", "matched_code": str(4 * done),
                                   "total_functions": 1, "matched_functions": done},
                                   "functions": [{"name": name.split("/")[1], "size": "4",
                                                  "fuzzy_match_percent": 100 if done else 90}]}
        report = {"units": [unit("Monsters/Ghoul 1", 1), unit("Monsters/Ghoul 2", 0), unit("Monsters/Bat", 1)]}
        lines = progress.subsystem_lines(report, {"monsters": ("Monsters", [])})
        self.assertIn("| **Gameplay** | 2/3 | 66.7% | `" + progress.bar(2 / 3, 20) + "` |", lines)
        self.assertIn("| **Ghoul** | 1/2 | 50.0% | `" + progress.bar(0.5, 20) + "` |", lines)
        self.assertIn("- Ghoul 2: `Ghoul 2` 4 B (90%)", lines)

    def test_every_game_function_has_a_unit_and_category(self):
        funcs, sizes = progress.load_functions()
        done = progress.collect_done(funcs, Path(progress.ROOT) / "src")
        pinned = progress.collect_pinned(funcs, Path(progress.ROOT) / "src") & done
        categories = progress.load_categories()
        report = progress.make_report(
            funcs, sizes, done, progress.load_libraries(funcs, sizes), pinned,
            progress.load_groups(funcs, sizes, progress.SUBSYSTEMS, "game"),
            areas=progress.load_areas(), asm_ranges=progress.load_asm_ranges(), categories=categories)
        labels = [label for label, _ in categories.values()]
        for unit in report["units"]:
            self.assertIn(unit["name"].split("/", 1)[0], labels)
            if not unit["name"].endswith(progress.ASM_SUFFIX):
                self.assertGreater(len(unit["functions"]), 1, unit["name"])
            top = [c for c in unit["metadata"]["progress_categories"] if c in categories]
            self.assertLessEqual(len(top), 1, unit["name"])
        self.assertNotIn("other", {c["id"] for c in report["categories"]},
                         "unclassified units: add a rule to symbols/progress_categories.csv")
        # Units are listed category by category, so each category is one region of the treemap.
        seen = []
        for unit in report["units"]:
            label = unit["name"].split("/", 1)[0]
            if not seen or seen[-1] != label:
                self.assertNotIn(label, seen)
                seen.append(label)
        self.assertEqual(sum(int(u["measures"]["total_code"]) for u in report["units"]), sum(sizes.values()))

    def test_library_assembly_ranges_leave_libraries_category(self):
        funcs = [("a", 0x08100104), ("b", 0x08100108), ("c", 0x0810010C)]
        sizes = {"a": 4, "b": 4, "c": 4}
        libs = [{"name": "SDK", "start": 0x08100104, "end": 0x08100110, "names": {}, "prefix": "libraries"}]
        areas = progress.load_areas()
        report = progress.make_report(funcs, sizes, {"a"}, libs, areas=areas,
                                      asm_ranges=[(0x08100108, 0x08100110)])
        cats = {c["id"]: c["measures"] for c in report["categories"]}
        self.assertEqual(cats["libraries"]["total_code"], "4")
        self.assertEqual(cats["intentional_asm"]["total_code"], "8")
        self.assertEqual(cats["decompilable"]["total_code"], "4")
        self.assertEqual(report["measures"]["total_code"], "4")  # headline leaves assembly out
        self.assertIn("libraries/SDK (assembly)", [u["name"] for u in report["units"]])

    def test_asm_ranges_load(self):
        self.assertTrue(progress.load_asm_ranges())

    def test_rom_data_units_cover_the_region_by_kind(self):
        import romdata
        units = romdata.measure()
        self.assertEqual(sum(u['size'] for u in units), romdata.ROM_DATA_END - romdata.ROM_DATA_START)
        kinds = {u['kind'] for u in units}
        self.assertEqual(kinds, {k for k, _ in romdata.KINDS})
        text = [u for u in units if u['kind'] == 'text']
        self.assertTrue(all(u['matched'] == u['size'] for u in text))

    def test_split_formats_and_decompiled_gcl_earn_nothing(self):
        import romdata
        with tempfile.TemporaryDirectory() as tmp:
            units, manifest, gcl = Path(tmp) / 'u.csv', Path(tmp) / 'm.csv', Path(tmp) / 'g.csv'
            end = romdata.ROM_DATA_END
            units.write_text('start,end,kind,unit,identified,evidence\n'
                             f'0824DAFC,08300000,tables,A,0,\n08300000,08400000,graphics,B,1048576,\n'
                             f'08400000,{end:08X},scripts,C,0,\n')
            manifest.write_text('name,start,end,format,file,description\n'
                                'reg,0824DB00,0824DC00,keyed_callbacks,x,\n'
                                'arc,08300000,08400000,resource_archive,x,\n')
            gcl.write_text('proc,start,size,status\n1,08400000,256,matched\n2,08400100,64,decompiled\n')
            got = {u['unit']: u for u in romdata.measure(units, manifest, gcl)}
        self.assertEqual(got['A']['matched'], 0x100)
        self.assertEqual(got['B']['matched'], 0)
        self.assertEqual((got['C']['matched'], got['C']['decompiled']), (256, 64))

    def test_rom_units_must_be_contiguous(self):
        import romdata
        with tempfile.TemporaryDirectory() as tmp:
            units = Path(tmp) / 'u.csv'
            units.write_text('start,end,kind,unit,identified,evidence\n0824DB00,08E88144,text,A,0,\n')
            with self.assertRaises(ValueError):
                romdata.load_units(units)

    def test_report_data_is_rom_only_and_categories_sum_to_headline(self):
        funcs = [('one', 0x08000100), ('two', 0x08000104)]
        report = progress.make_report(funcs, dict(one=4, two=4), {'one'})
        code = dict(report['measures'])
        progress.add_rom_data(report)
        m = report['measures']
        for key in ('total_code', 'matched_code', 'total_functions', 'total_units', 'complete_units'):
            self.assertEqual(m[key], code[key])
        data_units = [u for u in report['units'] if int(u['measures'].get('total_data', '0'))]
        for u in data_units:
            self.assertEqual(len(u['metadata']['progress_categories']), 1)
            self.assertEqual(u['measures']['total_code'], '0')
        cats = [c for c in report['categories'] if c['id'].startswith('rom_')]
        self.assertEqual(len(cats), 7)
        for key in ('total_data', 'matched_data'):
            self.assertEqual(sum(int(c['measures'][key]) for c in cats), int(m[key]))
            self.assertEqual(sum(int(u['measures'][key]) for u in data_units), int(m[key]))
        for c in cats:
            members = [u for u in data_units if u['metadata']['progress_categories'] == [c['id']]]
            self.assertEqual(sum(int(u['measures']['total_data']) for u in members), int(c['measures']['total_data']))
        import romdata
        self.assertEqual(int(m['total_data']), romdata.ROM_DATA_END - romdata.ROM_DATA_START)
        json.dumps(report)

if __name__ == '__main__':
    unittest.main()
