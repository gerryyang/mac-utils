[Pandoc](http://pandoc.org/index.html)

If you need to convert files from one markup format into another, `pandoc` is your `swiss-army knife`. Pandoc can convert documents in `markdown`, reStructuredText, textile, `HTML`, DocBook, `LaTeX`, MediaWiki markup, TWiki markup, OPML, Emacs Org-Mode, Txt2Tags, Microsoft Word docx, LibreOffice ODT, EPUB, or Haddock markup to


* HTML formats: XHTML, HTML5, and HTML slide shows using Slidy, reveal.js, Slideous, S5, or DZSlides.
* Word processor formats: Microsoft Word docx, OpenOffice/LibreOffice ODT, OpenDocument XML
* Ebooks: EPUB version 2 or 3, FictionBook2
* Documentation formats: DocBook, GNU TexInfo, Groff man pages, Haddock markup
* Page layout formats: InDesign ICML
* Outline formats: OPML
* TeX formats: LaTeX, ConTeXt, LaTeX Beamer slides
* PDF via LaTeX
* Lightweight markup formats: Markdown (including CommonMark), reStructuredText, AsciiDoc, MediaWiki markup, DokuWiki markup, Emacs Org-Mode, Textile
* Custom formats: custom writers can be written in lua.


[http://pandoc.org/demos.html](http://pandoc.org/demos.html)

From markdown to PDF:

```
pandoc README.md --latex-engine=xelatex -o README.pdf
```

Online pandoc

[http://pandoc.org/try/](http://pandoc.org/try/)

---------
