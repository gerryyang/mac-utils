

Run pdflatex latex-fourth and look at the PDF file. Notice that the references that we just entered didn’t work — they appear as question marks. As LATEX runs it saves labels to a file. When you run a file with a first-time label it has not yet been saved. The question marks go away when you run pdflatex latex-fourth a second time.

(When you are writing a real document, because you are fixing typos, etc., you will run LATEX a number of times, so in practice having to rerun the command isn’t an issue.)

