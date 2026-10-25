# Boktai 3 (U33J) reverse-engineering build.
#   make setup     fetch/build pinned tools (gbadisasm, agbcc, armips, mGBA harness)
#   make disasm    regenerate gen/code.s from baserom.gba + symbols/
#   make           assemble build/boktai3.gba and verify it matches the base ROM
#   make mod       build build/mod.gba from edited data (text: build/text/bank.tsv)
#   make english   build build/english.gba from the fan translation (../boktai3trans)
#   make format    clang-format our C (make format-check: fail if unformatted)
AS      := arm-none-eabi-as
LD      := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
ASFLAGS := -mcpu=arm7tdmi --no-warn
PY      := python3

ROM     := build/boktai3.gba
SHA1    := 2651c5e6875ac60abff734510d152166d211c87c

.PHONY: all compare setup disasm clean shifttest format format-check mod english
# Default: split asm around src/*.c, compile with agbcc, link, verify SHA-1
all: gen/code_sym.s gen/data.s
	$(PY) tools/build.py
	$(PY) tools/check_ram_literals.py

# Modified ROM from edited sources (e.g. build/text/bank.tsv): no SHA-1 check
mod: gen/code_sym.s gen/data.s
	$(PY) tools/build.py --out build/mod.gba --no-compare
	@echo build/mod.gba

# English ROM from the fan translation (clone github.com/moozilla/boktai3trans
# next to this repository, or set TRANS=path): build/english.gba
english: gen/code_sym.s gen/data.s
	$(PY) tools/trans/english.py $(if $(TRANS),--trans $(TRANS))

setup:
	tools/setup.sh

gen/code.s: tools/disasm.py symbols/functions.csv symbols/progress_functions.csv
	$(PY) tools/disasm.py

gen/code_sym.s gen/data.s: gen/code.s tools/symbolize.py tools/m4a.py symbols/*.csv symbols/*.txt data/manifest.csv $(wildcard data/*.csv) $(wildcard build/text/*.tsv) $(wildcard build/res/*/*) $(wildcard tools/datafmt/*.py)
	$(PY) tools/symbolize.py

disasm: gen/code_sym.s

build/rom.o: asm/rom.s asm/macros.inc gen/code_sym.s gen/data.s baserom.gba
	$(AS) $(ASFLAGS) -o $@ asm/rom.s

# Shiftability test: move all data by SHIFT bytes and boot it in the emulator
build/shifted.gba: asm/rom.s asm/macros.inc gen/code_sym.s gen/data.s baserom.gba
	$(AS) $(ASFLAGS) --defsym SHIFT=$(or $(SHIFT),0x100) -o build/shifted.o asm/rom.s
	$(LD) -Ttext=0x08000000 -e 0x08000000 -o build/shifted.elf build/shifted.o
	$(OBJCOPY) -O binary build/shifted.elf $@

build/rom.elf: build/rom.o
	$(LD) -Ttext=0x08000000 -e 0x08000000 -o $@ $<

$(ROM): build/rom.elf
	$(OBJCOPY) -O binary $< $@

# Pure-asm build (no C), useful to check the generated asm alone
compare: $(ROM)
	@$(PY) -c "import hashlib; p='$(ROM)'; ok=hashlib.sha1(open(p,'rb').read()).hexdigest()=='$(SHA1)'; print(p+(': OK' if ok else ': MISMATCH')); raise SystemExit(not ok)"

clean:
	rm -f build/rom.o build/rom.elf $(ROM)

# Regression: data moved by SHIFT must play identically (needs the harness)
SAVE := tests/saves/ShinBok2.sav
shifttest: gen/code_sym.s gen/data.s
	tools/shift_test.sh tests/newgame_intro.txt $(or $(SHIFT),0x10000)
	BOKTAI3_SAV=$(SAVE) tools/shift_test.sh tests/save_boot.txt $(or $(SHIFT),0x10000)
	BOKTAI3_SAV=$(SAVE) tools/shift_test.sh tests/save_menus_field.txt $(or $(SHIFT),0x10000)

# Our C sources (pret/SDK-derived src/lib, include/gba, rfu_compat.h keep upstream layout).
CLANG_FORMAT := $(or $(wildcard build/venv/bin/clang-format),clang-format)
FORMAT_SRCS = $(filter-out src/lib/% include/gba/% include/rfu_compat.h,$(wildcard src/*.c src/*/*.c include/*.h include/*/*.h))
format:
	@$(CLANG_FORMAT) -i $(FORMAT_SRCS)
format-check:
	@$(CLANG_FORMAT) --dry-run --Werror $(FORMAT_SRCS)
