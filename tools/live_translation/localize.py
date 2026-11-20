import hashlib
import json
import re
from urllib.error import URLError
from context import Catalog, BANK, BASE
from runtime import text_bank
from translator import CONTROLS, fit


class Localizer:
    """One resolver for every guest encounter. English pointers live until reset."""
    def __init__(self, memory, transcript, enabled=True):
        self.memory = memory
        self.transcript = transcript
        self.enabled = enabled
        self.catalog = None
        self.pointers = {}
        self.seen = set()
        self.counts = {}
        self.graphics = None

    def record(self, key, **fields):
        identity = (key, fields.get("event"), fields.get("scene"), fields.get("kind"),
                    fields.get("caller_pc"), fields.get("result"))
        if identity in self.seen:
            return
        self.seen.add(identity)
        self.transcript.write(json.dumps(dict(key=key, **fields), ensure_ascii=False)+"\n")
        self.transcript.flush()

    def __call__(self, game, hook, arg, ptr, aux, lr, scene, proc, frame):
        if not self.catalog:
            self.catalog = Catalog(game.rom)
        c = self.catalog
        if hook == 3:
            if self.graphics:
                return self.graphics(self, game, arg, ptr, aux, lr, scene, proc, frame)
            return 0
        if hook == 1:
            sid = arg
            source = c.text(sid)
            if source is None:
                return 0
            kind = "dialogue" if lr == 0x080313DE else "text"
            key = f"text/{sid}"
        else:
            sid = c.ids.get(ptr, self.pointers.get(ptr))
            if sid is not None:
                key, source = f"text/{sid}", c.text(sid)
            else:
                if not (0x02000000<=ptr<0x02040000 or 0x03000000<=ptr<0x03008000 or BASE<=ptr<0x09000000):
                    return 0
                raw = game.read(ptr,384).partition(b'\0')[0]
                if len(raw)==384:
                    return 0
                source = c.decode(raw)
                key = "literal/"+hashlib.sha256(raw).hexdigest()[:16]
            kind = "name" if hook==4 else "menu"
        if not source or not re.search(r"[^\x00-\x7f]",CONTROLS.sub("",source)):
            return 0
        ctx = c.context(game,scene,proc,lr,kind,source,sid,arg if hook==2 else 0,aux if hook==2 else None)
        if kind=="name":
            # A custom save's player name belongs to the player; render only
            # explicitly approved spellings in the local glossary.
            if source not in self.memory.glossary():
                return 0
        word = origin = rendered = None
        try:
            if self.enabled:
                word,origin,event = self.memory.choose(key,source,ctx)
            else:
                event = "japanese"
            if word:
                rendered = fit(source,word,ctx)
        except (ValueError, OSError, URLError) as error:
            event = "needs-review"
            self.record(key,event=event,reason=str(error),source=source,frame=frame,**ctx)
            return 0
        self.record(key,event=event,source=source,english=word,rendered=rendered,
                    provenance=origin,frame=frame,**ctx)
        if rendered is None:
            return 0
        address = game.install(text_bank.encode_string(rendered,{})+b'\0')
        if sid is not None:
            self.pointers[address]=sid
        return address
