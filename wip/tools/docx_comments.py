"""
Lists all comments and tracked changes of a reviewed Word file, read directly from the docx XML.
(pandoc drops comments anchored in code blocks, so don't rely on it for the review.)

    python tools/docx_comments.py review/<file>.docx
"""
import html
import re
import sys
import zipfile


def text_of(xml: str) -> str:
    return html.unescape("".join(re.findall(r"<w:t[^>]*>([^<]*)</w:t>", xml)))


def main():
    z = zipfile.ZipFile(sys.argv[1])
    doc = z.read("word/document.xml").decode("utf-8")
    comments = z.read("word/comments.xml").decode("utf-8") if "word/comments.xml" in z.namelist() else ""
    n = 0
    for m in re.finditer(r'<w:comment [^>]*w:id="(\d+)"[^>]*>(.*?)</w:comment>', comments, re.S):
        cid = m.group(1)
        a = re.search(r'<w:commentRangeStart w:id="%s"/>(.*?)<w:commentRangeEnd w:id="%s"/>' % (cid, cid), doc, re.S)
        anchor = text_of(a.group(1)) if a else "?"
        print(f"[comment {cid}] on: {anchor[:100]}\n    {text_of(m.group(2))}\n")
        n += 1
    for kind, tag in (("insert", "w:ins"), ("delete", "w:del")):
        for m in re.finditer(r"<%s [^>]*>(.*?)</%s>" % (tag, tag), doc, re.S):
            t = text_of(m.group(1)) or html.unescape("".join(re.findall(r"<w:delText[^>]*>([^<]*)</w:delText>", m.group(1))))
            # context: text of the paragraph around the change
            start = doc.rfind("<w:p ", 0, m.start()); end = doc.find("</w:p>", m.end())
            print(f"[{kind}] '{t}' in: {text_of(doc[start:end])[:120]}\n")
            n += 1
    print(f"{n} items")


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    main()
