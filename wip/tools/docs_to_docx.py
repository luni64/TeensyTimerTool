"""
Exports the MkDocs documentation of TeensyTimerTool (lib/TeensyTimerTool/docs, order from mkdocs.yml)
into one Word file for review. MkDocs specifics are converted for Word:
  --8<-- "file"        example code is inlined from examples/
  !!! type "Title"     admonitions become a block quote with a bold title
  === "Tab"            content tabs become bold paragraphs
  { .annotate }        the annotation list below a paragraph becomes block quotes "(1) ..."
Each chapter starts with its source file name, so comments can be mapped back to the sources.

    python tools/docs_to_docx.py [chapter.md ...]     # default: all chapters in nav order
    -> review/TeensyTimerTool-docs-<date>.docx

Reading the comments back: pandoc review/<file>.docx -t markdown --track-changes=all
"""
import datetime
import re
import subprocess
import sys
from pathlib import Path

DEV = Path(__file__).resolve().parents[1]
# dev root layout: <dev>/tools + <dev>/lib/TeensyTimerTool; WIP copy in the repo: <repo>/wip/tools (output: wip/review)
LIB = DEV.parent if (DEV.parent / "mkdocs.yml").exists() else DEV / "lib" / "TeensyTimerTool"
DOCS = LIB / "docs"
OUT = DEV / "review"


def nav_files() -> list[str]:
    files = []
    for line in (LIB / "mkdocs.yml").read_text(encoding="utf-8").splitlines():
        m = re.match(r"\s*-\s*(?:[^:]+:\s*)?([\w/.-]+\.md)\s*$", line)
        if m:
            files.append(m.group(1))
    return files


def convert(md: str) -> str:
    # inline example code
    md = re.sub(r'^--8<--\s*"([^"]+)"\s*$',
                lambda m: (LIB / "examples" / m.group(1)).read_text(encoding="utf-8").rstrip(),
                md, flags=re.M)
    out, lines, i = [], md.splitlines(), 0
    while i < len(lines):
        line = lines[i]
        m = re.match(r'^(\s*)!!!\s+(\w+)(?:\s+inline(?:\s+end)?)?(?:\s+"([^"]*)")?\s*$', line)
        t = re.match(r'^(\s*)===\s+"([^"]*)"\s*$', line)
        if re.match(r'^\s*\{\s*\.annotate\s*\}\s*$', line):
            # annotations: the "(n)" markers stay in the text, each list item "n.  ..." below
            # becomes a block quote "(n) ...", like the admonitions
            i += 1
            out.append("")
            while i < len(lines) and not lines[i].strip():
                i += 1
            while i < len(lines):
                a = re.match(r'^(\d+)\.\s+(.*)$', lines[i])
                if not a:
                    break
                body = [f"({a.group(1)}) {a.group(2)}"]
                i += 1
                while i < len(lines) and (not lines[i].strip() or lines[i].startswith("    ")):
                    body.append(lines[i][4:] if lines[i].strip() else "")
                    i += 1
                while body and not body[-1]:
                    body.pop()
                out += [f"> {b}" if b else ">" for b in body] + [""]
            continue
        if m or t:
            indent = len((m or t).group(1)) + 4
            body = []
            i += 1
            while i < len(lines) and (not lines[i].strip() or len(lines[i]) - len(lines[i].lstrip()) >= indent):
                body.append(lines[i][indent:] if lines[i].strip() else "")
                i += 1
            if m:
                title = m.group(3) or m.group(2).capitalize()
                out += [f"> **{title}**", ">"] + [f"> {b}" if b else ">" for b in body] + [""]
            else:
                out += [f"**{t.group(2)}**", ""] + body + [""]
            continue
        out.append(line)
        i += 1
    return "\n".join(out)


def main():
    files = sys.argv[1:] or nav_files()
    parts = []
    for f in files:
        md = convert((DOCS / f).read_text(encoding="utf-8"))
        md = re.sub(r"^(#+ .*)$", r"\1", md, count=1, flags=re.M)
        parts.append(md + f"\n\n*Source: docs/{f}*\n\n")
    OUT.mkdir(exist_ok=True)
    stamp = datetime.date.today().isoformat()
    tmp = OUT / f"_review-{stamp}.md"
    tmp.write_text("\n\n\\newpage\n\n".join(parts), encoding="utf-8")
    docx = OUT / f"TeensyTimerTool-docs-{stamp}.docx"
    n = 2
    while docx.exists():  # never overwrite a reviewed file
        docx = OUT / f"TeensyTimerTool-docs-{stamp}-{n}.docx"
        n += 1
    subprocess.run(["pandoc", str(tmp), "-f", "markdown-smart", "-o", str(docx), "--toc",
                    "--metadata", f"title=TeensyTimerTool documentation (review {stamp})",
                    "--syntax-highlighting=tango"], check=True)
    tmp.unlink()
    print(docx)


if __name__ == "__main__":
    main()
