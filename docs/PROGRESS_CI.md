# ROM-free progress reports

`.github/workflows/progress.yml` runs on pushes, pull requests and manual
requests. It needs Python only, no ROM, compiler, secret, or generated assembly.
It tests progress accounting, renders a job summary and uploads exactly one
file, `report.json`, in the artifact `jp_report`. Artifact retention is 30 days
with maximum compression to keep storage small.
No ROM, instructions, build outputs or generated assembly are uploaded.

## Counting policy

`symbols/progress_functions.csv` is a reviewed list of function-start addresses,
with no instructions or byte contents. Names come from `symbols/functions.csv`
or the normal `sub_XXXXXXXX` address name. A function's progress size runs to the
next start, with the last ending at `0824DAFA`, preserving the historical
`progress.py` accounting (including intervening alignment and literal pools).
This measures the code region, not the whole ROM or asset progress.

Three reviewed SDK entries in `symbols/progress_extra_functions.csv` are also
included. They already have verified C definitions in `src/lib/m4a.c`, but the
disassembler absorbs their bytes into preceding function spans. The corrected
baseline is **4,145 / 11,028 functions**, versus the historical 4,145 / 11,025:
the old numerator counted these three while its denominator did not. Code-byte
progress is unchanged. Refreshing metadata unions these explicit boundaries
with generated starts; it does not infer new boundaries from arbitrary C.

The same `split.c_functions` scanner used by the build identifies non-static C
definitions and address aliases. `INCLUDE_ASM` placeholders and static helpers
do not count. Counts include only known boundaries, deduplicate names, and fail
on an unknown definition instead of silently inflating progress. Each function
is one report unit because original translation-unit boundaries are unknown.

CI reports C coverage under the project's rule that source is committed only
after a byte-identical local build. CI itself does **not** verify matching: a
source edit that violates that rule can produce an overstated report. Reviewers
must retain the local `make`/`tools/check.py` verification requirement. The report
uses objdiff v2 JSON, decimal strings for uint64 byte counts and addresses, and
numbers for function counts and percentages. Completed code equals matched C;
assembly placeholders receive zero credit.

## Local use and refreshing metadata

From the repository root:

```sh
python3 -m unittest discover -s tests/tools -p 'test_progress.py' -v
python3 tools/progress.py --json /tmp/report.json --markdown /tmp/PROGRESS.md
python3 tools/progress.py --check-inventory gen/code_sym.s
```

After a disassembly update changes function boundaries:

```sh
python3 tools/progress.py --refresh-inventory gen/code_sym.s
python3 tools/progress.py --check-inventory gen/code_sym.s
make
```

Review the address-only CSV diff and commit it with the boundary change only
after `make` prints `build/boktai3.gba: OK`. Never add `gen/code_sym.s` or a ROM.
A name-only change in `symbols/functions.csv` needs no inventory refresh.

## Registering the public project

1. Land this workflow and metadata on the default branch of
   [moozilla/boktai3-decomp](https://github.com/moozilla/boktai3-decomp), then let
   a **push-triggered** run finish successfully with a `jp_report` artifact.
   Manual runs alone do not satisfy initial workflow discovery.
2. Sign in to [decomp.dev](https://decomp.dev) with a GitHub account that has
   admin permission on the repository and visit
   [Add project](https://decomp.dev/manage/new).
3. Enter the repository URL, choose Game Boy Advance and the desired display
   name. Select Japanese version `jp` if prompted; the artifact prefix defines
   that version. Verify that the initial totals and history are displayed.
4. Optionally install the decomp-dev GitHub App for automatic updates and pull
   request reporting. The public Actions artifact workflow needs no repository
   secrets. Registration and App installation are account actions, separate
   from generating reports.

Reports are pulled by decomp.dev from GitHub Actions; this workflow does not
push to a decomp.dev API. Expired artifacts cannot be used to backfill history,
so register after the first successful default-branch push.

## Integration references

- [Decompedia integration guide](https://decomp.wiki/tools/decomp-dev).
- [objdiff report schema](https://github.com/encounter/objdiff/blob/main/objdiff-core/protos/report.proto).
- [decomp.dev artifact parsing and workflow discovery](https://github.com/encounter/decomp.dev/blob/e9c086adb74d2fe569541cd715cd9312d7641313/crates/github/src/lib.rs).
- [decomp.dev registration permission check](https://github.com/encounter/decomp.dev/blob/e9c086adb74d2fe569541cd715cd9312d7641313/crates/web/src/handlers/manage.rs).

The primary source links were inspected on 2026-10-08. The server recognizes
`<version>_report`/`<version>-report` artifacts and report/progress file stems,
parses the objdiff report and requires version 2. This project uses the simplest
contract: `jp_report` containing `report.json`.
