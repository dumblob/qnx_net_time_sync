# make [  | all | clean ]

DOCNAME = bc_thesis_Jan_Pacner_2013
CONTENT = $(DOCNAME)_content
HEADER_PATH = sablona/tex/

all: folder header content
	pdfjam -o $(DOCNAME).pdf \
		--fitpaper true \
		--rotateoversize true \
		-- $(HEADER_PATH)/projekt.pdf - $(CONTENT).pdf -

folder:

header:
	make -C $(HEADER_PATH) > /dev/null

#FIXME add backref from hyperref package to citations
# article | report | book | memoir | letter | slides (transparent folie)
# date "+%d.%m.%Y"
content:
	pandoc -S -s --toc -N --section-divs --no-tex-ligatures \
		--variable lang=czech \
		--variable date="$(shell date "+%x")" \
		--variable geometry=a4paper \
		--variable geometry='total={15.2cm,23cm}' \
		--variable geometry=top=2.4cm \
		--variable geometry=left=3.3cm \
		--variable fontsize=11pt \
		--variable documentclass=report \
		-f markdown -t latex -o - $(CONTENT).markdown | \
		sed -r 's/\(([0-9]+)\)/{[}\1{]}/g' > $(CONTENT).tex
	pdflatex $(CONTENT).tex > /dev/null
	pdflatex $(CONTENT).tex > /dev/null
	rm -f \
		$(CONTENT).tex \
		$(CONTENT).aux \
		$(CONTENT).log \
		$(CONTENT).out \
		$(CONTENT).toc

clean:
	make -C $(HEADER_PATH) clean
	rm -f $(CONTENT).pdf $(DOCNAME).pdf

