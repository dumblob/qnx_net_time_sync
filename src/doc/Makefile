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
_content:
	pandoc -S -s --toc -N --section-divs --no-tex-ligatures \
		--template template.latex \
		--variable lang=czech \
		--variable date="$(shell date "+%x")" \
		--variable geometry=a4paper \
		--variable geometry='total={15.2cm,23cm}' \
		--variable geometry=top=2.4cm \
		--variable geometry=left=3.3cm \
		--variable fontsize=11pt \
		--variable documentclass=report \
		-f markdown -t latex -o - $(CONTENT).markdown | \
		sed -r 's/\(([0-9]+)\)/{[}\1{]}/g' | \
		awk -F'\n' '{ \
			if ($$0 ~ /\\def\\labelenumi[{][(]\\arabic[{]enumi[}][)][}]/) { \
				print last_match content; \
				last_match = $$0; \
				content = "" \
			} \
			else { \
				content = content "\n" $$0 \
			} \
		} \
		END { \
			print "\\def\\labelenumi{{[}\\arabic{enumi}{]}}"; \
			print content \
		}' | \
		sed -r 's|(\\includegraphics)(\{fig/ntppacket00.png\})|\1[width=80mm]\2|' \
		> $(CONTENT).tex
	pdflatex -interaction batchmode -shell-escape $(CONTENT).tex > /dev/null || { \
		echo 'Look at *.log and try to correct the escaping mistakes etc.'; false; }
	pdflatex -shell-escape $(CONTENT).tex > /dev/null

content: _content clean

clean:
	rm -f \
		$(CONTENT).aux \
		$(CONTENT).dvi \
		$(CONTENT).log \
		$(CONTENT).out \
		$(CONTENT)-pics.pdf \
		$(CONTENT).tex \
		$(CONTENT).toc

distclean: clean
	make -C $(HEADER_PATH) clean
	rm -f $(CONTENT).pdf $(DOCNAME).pdf
