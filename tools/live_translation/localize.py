import hashlib
import json
import re
from urllib.error import URLError
from context import Catalog, BASE
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
        self.looked_up = {}
        self.seen = set()
        self.dialogue_id = None
        self.bookmarks = False
        self.line = None
        self.source_preview = None
        self.graphics = None
        from backgrounds import MapTranslator
        self.backgrounds = MapTranslator()

    def record(self, key, **fields):
        identity = (key, fields.get("event"), fields.get("scene"), fields.get("kind"),
                    fields.get("caller_pc"), fields.get("english"), fields.get("digest"), fields.get("reason"))
        if identity in self.seen:
            return
        self.seen.add(identity)
        self.transcript.write(json.dumps(dict(key=key, **fields), ensure_ascii=False)+"\n")
        self.transcript.flush()

    def current_line(self, game):
        if not self.line or self.line["id"] != self.dialogue_id:
            return None
        scene = int.from_bytes(game.read(0x030039F0, 2), "little")
        window = int.from_bytes(game.read(0x020000DC, 4), "little")
        if self.line["context"]["scene"] == scene and self.line["window"] == window:
            return self.line
        return None

    def __call__(self, game, hook, arg, ptr, aux, lr, scene, proc, frame):
        if not self.catalog:
            self.catalog = Catalog(game.rom)
        c = self.catalog
        if hook == 7:
            return self.graphics.select(self, game, arg, ptr, aux, lr, scene, proc, frame) if self.graphics else 0
        if hook in (6, 10):
            return self.backgrounds.select(self, game, arg, ptr, aux, lr, scene, proc, frame, hud=hook==10)
        if hook == 3:
            return 0
        if hook == 1:
            sid = arg
            source = c.text(sid)
            if source is None:
                return 0
            # The bank also supplies save names, password alphabets, and small
            # temporary copy buffers. Remember their IDs; translate only when
            # a native display consumer is requesting the pointer.
            if lr not in (0x080313DE, 0x08033604):
                self.looked_up[source] = sid
                return 0
            kind = "dialogue" if lr == 0x080313DE else "window"
            if kind == "dialogue":
                self.dialogue_id = sid
                if sid != self.source_preview:
                    self.source_preview = None
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
                sid = self.looked_up.get(source)
                key = f"text/{sid}" if sid is not None else "literal/"+hashlib.sha256(raw).hexdigest()[:16]
            kind = {2:"menu",4:"name",5:"ticker",8:"substitution",9:"direct-playback"}[hook]
        if not source or not source.strip() or not re.search(r"[^\x00-\x7f]",CONTROLS.sub("",source)):
            return 0
        ctx = c.context(game,scene,proc,lr,kind,source,sid,arg if hook==2 else 0,aux if hook==2 else None)
        if hook==1 and 0x02000000<=aux<0x02040000:
            x,y,w,h=game.read(aux+4,4)
            if 1<=w<=30 and 1<=h<=20:
                ctx.update(window=[x,y,w,h],columns=w,lines=max(1,h//2))
        if kind == "name" or (kind == "menu" and ctx["caller"] == "GameStart_DrawSaveInfo" and (arg, aux) == (8, 7)):
            # A custom save's player name belongs to the player; render only
            # explicitly approved spellings in the local glossary.
            if source not in self.memory.glossary():
                return 0
        if self.bookmarks and kind == "dialogue" and ctx.get("lines", 0) > 1:
            if game.core.lt_bookmark():
                self.line = {"key": key, "id": sid, "source": source, "context": ctx, "frame": frame,
                             "window": int.from_bytes(game.read(0x020000DC, 4), "little")}
        enabled = self.enabled and not (self.source_preview is not None and
                                        (sid == self.source_preview or kind == "name"))
        word = origin = rendered = None
        try:
            if enabled:
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
        if self.line and self.line["key"] == key:
            self.line.update(english=word, provenance=origin)
        if rendered is None:
            return 0
        try:
            address = game.install(text_bank.encode_string(rendered,{})+b'\0')
        except ValueError as error:
            self.record(key,event="needs-review",reason=str(error),source=source,frame=frame,**ctx)
            return 0
        if sid is not None:
            self.pointers[address]=sid
        return address
