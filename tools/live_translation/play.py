#!/usr/bin/env python3
"""Run the in-engine English translation or inspect its local translation memory."""
import argparse
from contextlib import closing
import json
from pathlib import Path
import sys
from runtime import Game, OUT, prison_save
from translator import Memory, Ollama
from localize import Localizer


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--db", type=Path, default=OUT/"translations.sqlite")
    parser.add_argument("--model", help="Enable an Ollama model by name. Default uses local answers.")
    parser.add_argument("--endpoint", default="http://127.0.0.1:11434")
    parser.add_argument("--japanese", action="store_true")
    sub=parser.add_subparsers(dest="cmd")
    sub.add_parser("replay")
    sub.add_parser("review").add_argument("key", nargs="?")
    ed=sub.add_parser("correct"); ed.add_argument("key"); ed.add_argument("english")
    term=sub.add_parser("term");term.add_argument("japanese");term.add_argument("english")
    a=parser.parse_args()
    OUT.mkdir(parents=True,exist_ok=True)
    m=Memory(a.db,Ollama(a.endpoint,a.model) if a.model else None)
    if a.cmd=="correct":
        m.correct(a.key,a.english); print("Correction saved for",a.key);return
    if a.cmd=="review":
        for r in m.db.execute("SELECT * FROM entries WHERE ? IS NULL OR key=? ORDER BY key",(a.key,a.key)):
            print(f'{r["key"]}  [{r["origin"]}]\n{r["source"]}\n{r["english"] or "(awaiting translation)"}')
            if a.key: print(json.dumps(json.loads(r["context"]),ensure_ascii=False,indent=2))
        return
    if a.cmd=="term":
        with m.db:
            m.db.execute("INSERT INTO glossary VALUES (?,?) ON CONFLICT(japanese) DO UPDATE SET english=excluded.english",(a.japanese,a.english))
        print("Glossary updated");return
    target=OUT/("japanese" if a.japanese else "english")
    target.mkdir(exist_ok=True)
    with (target/"transcript.jsonl").open("w") as log:
        loc=Localizer(m,log,not a.japanese)
        with closing(Game(loc,prison_save())) as game:
            for i,(n,key) in enumerate([(300,"START"),(60,"A"),(150,"START"),(120,"A"),(120,"START"),
                                        (300,"START"),(120,"START"),(120,"A"),(150,"A")]+[(100,"A")]*24):
                game.step(n)
                if i in (6,7,14,15,19,20,21,25,26,30): game.image().save(target/f"{i:02}.png")
                game.tap(key)
    print(target)

if __name__=="__main__":
    main()
