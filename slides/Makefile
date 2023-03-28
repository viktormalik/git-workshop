slides: git-figures/*.pdf
	xelatex slides.tex

%.pdf: %.html
	 wkhtmltopdf --enable-local-file-access $^ $@
	 pdfcrop $@ $@
