PANDOC = pandoc
RM = rm -f
METADATA_FILE = metadata.yaml
PDF_FILE = install_instructions.pdf
DOCX_FILE = install_instructions.docx
TARGET_FILES = ${PDF_FILE} ${DOCX_FILE}

.phony: clean pdf docx

all: pdf docx

pdf: ${PDF_FILE}

docx: ${DOCX_FILE}

%.docx:%.md
	${PANDOC} -o $@ -s $<

%.pdf:%.md ${METADATA_FILE}
	${PANDOC} --pdf-engine=xelatex -o $@ -s $^

clean:
	${RM} ${TARGET_FILES}