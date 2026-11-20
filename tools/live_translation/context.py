import bisect
import csv
import hashlib
import re
import struct
from functools import lru_cache
from runtime import ROOT, text_bank
import gcx

BASE = 0x08000000
TABLE, BANK = text_bank.TABLE, text_bank.BANK


class Catalog:
    def __init__(self, rom):
        self.rom = rom
        self.cmap = text_bank.charmap()
        self.words = struct.unpack_from(f"<{(BANK-TABLE)//4}I", rom, TABLE-BASE)
        self.ids = {BANK+(word & 0x7fffffff): i for i, word in enumerate(self.words) if word >> 31}
        self.rom_digest = hashlib.sha256(rom).hexdigest()
        with (ROOT / "data/labels/scenes.csv").open() as f:
            self.scenes = {int(r["scene"]): r for r in csv.DictReader(f)}
        self.script = gcx.Image(rom)
        self.procs = sorted((BASE+self.script.proc_address(i), i+1) for i,v in enumerate(self.script.procs) if v)
        with (ROOT / "symbols/functions.csv").open() as f:
            self.functions = sorted((int(r["addr"],16),r["name"]) for r in csv.DictReader(f))
        self.entry_names = dict(self.functions)
        with (ROOT / "gcl/names/proc.csv").open() as f:
            self.names = {int(r["proc"]):r["name"] for r in csv.DictReader(f)}

    def decode(self, data):
        return text_bank.decode_string(data, self.cmap).replace(r"\n", "\n")

    @lru_cache(maxsize=1024)
    def text(self, i):
        if not 0 <= i < len(self.words) or not self.words[i] >> 31:
            return None
        start = BANK-BASE+(self.words[i] & 0x7fffffff)
        end = self.rom.index(0,start)
        return self.decode(self.rom[start:end])

    def caller(self, lr):
        i = bisect.bisect_left(self.functions,(lr, ""))-1
        return self.functions[i][1] if i >= 0 and lr-self.functions[i][0] < 0x10000 else f"{lr:08x}"

    @lru_cache(maxsize=1024)
    def script_context(self, pc):
        i = bisect.bisect_right(self.procs, (pc, 1<<24))-1
        if i < 0:
            return None
        addr, pid = self.procs[i]
        p, length = gcx.length(self.rom,addr-BASE)
        end = p+length
        if not p <= pc-BASE < end:
            return None
        while p<end and self.rom[p]:
            start, n = gcx.length(self.rom,p)
            if start+n > pc-BASE:
                lines = []
                for _ in range(3):
                    if p>=end or not self.rom[p]:
                        break
                    a, k = gcx.length(self.rom,p)
                    lines.extend(t for _,t in gcx.instructions(self.rom,p,a+k)[:5])
                    p = a+k
                return {"procedure":pid, "name":self.names.get(pid), "at_and_after":lines[:9]}
            p = start+n
        return None

    def callback(self, ptr):
        if BASE <= ptr < BASE + len(self.rom):
            addr = ptr & ~1
            return self.entry_names.get(addr, f"sub_{addr:08X}")
        return None

    def interface(self, game):
        task = game.core.lt_task()
        callback = self.callback(game.core.lt_callback())
        result = {"active_task": callback} if callback else {}
        hud = int.from_bytes(game.read(0x02000214, 4), "little")  # gFieldHud
        if 0x02000000 <= hud <= 0x02040000 - 0x1415:
            # SceneDialog_ConfigureVariantSlot dispatches on SceneHudState.mode;
            # the page and its input callback can change within the same room.
            update, page = struct.unpack("<II", game.read(hud + 0xA50, 8))
            mode = game.read(hud + 0x1414, 1)[0]
            result["hud"] = {"update": self.callback(update), "page_callback": self.callback(page)}
            if mode < 10 and page and (task == hud or result["hud"]["update"] == "SceneHud_UpdatePageInput"):
                tabs = ("Map", "Items", "Valuables", "Gun Del Sol", "Swords",
                        "Accessories", "Status", "Settings", "Sleep", "Save")
                init = struct.unpack_from("<I", self.rom, 0x611C34 + mode*4)[0]
                result["menu"] = {"tab": tabs[mode], "index": mode,
                                  "page_initializer": self.callback(init), **result["hud"]}
        return result

    def context(self, game, scene, recent_proc, lr, kind, source, key, col=0, row=None):
        ctx = dict(kind=kind, scene=scene, scene_name=self.scenes.get(scene, {}).get("place") or self.names.get(scene), caller=self.caller(lr),
                   caller_pc=f"{lr:08X}", last_called_proc=recent_proc,
                   columns=30, lines=3, name_cells=8)
        ctx.update(self.interface(game))
        ctx["rom_sha256"] = self.rom_digest
        pc = int.from_bytes(game.read(0x02000610,4),"little")
        ctx["script_cursor"] = f"{pc:08X}"
        ctx["script"] = self.script_context(pc-1)
        if kind=="menu":
            ctx.update(column=col, row=row, columns=max(1,30-col),lines=max(1,(20-row)//2))
        if kind=="dialogue":
            address = int.from_bytes(game.read(0x020000DC,4),"little")
            if 0x02000000 <= address <= 0x0203fe00:
                x,y,w,h = game.read(address+0x17c,4)
                if 1 <= w <= 30 and 1 <= h <= 20:
                    ctx.update(window=[x,y,w,h], columns=w, lines=max(1,h//2))
                ctx["portrait_slot"] = int.from_bytes(game.read(address+0x18+0x15,1),"little",signed=True)
            # Identify the active conversation's explicit label, including labels
            # in earlier messages of a multi-line speech.
            for sid in range(key,max(-1,key-18),-1):
                text = self.text(sid) or ""
                if sid != key and "<END>" in text:
                    break
                label = re.search(r"<LABEL=([^>]+)>",text)
                if label:
                    ctx["speaker_label"] = label[1]
                    ctx["speaker_label_text_id"] = sid
                    break
            ctx["next_bank_lines"] = [self.text(i) for i in range(key+1,min(key+3,len(self.words)))]
        return ctx
