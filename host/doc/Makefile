PANDOC = pandoc
RM = rm -f
METADATA_FILE = metadata.yaml
PDF_FILE = install_instructions.pdf jobs_json.pdf
DOCX_FILE = install_instructions.docx jobs_json.docx
TARGET_FILES = ${PDF_FILE} ${DOCX_FILE}

.phony: clean pdf docx

all: install_instructions.pdf install_instructions.docx

jobs_doc : jobs_json.pdf jobs_json.docx

pdf: ${PDF_FILE}

docx: ${DOCX_FILE}

%.docx:%.md
	${PANDOC} -o $@ -s $<

%.pdf:%.md ${METADATA_FILE}
	${PANDOC} --pdf-engine=xelatex -o $@ -s $^

jobs_json.html: jobs.schema.json
	generate-schema-doc --no-copy-css --no-copy-js $^ $@

jobs_json.md: jobs_json.html
	${PANDOC} -o $@ $^

jobs.schema.json:
	Lr1110FieldTest -j > $@

clean:
	${RM} ${TARGET_FILES} jobs_json.html jobs_json.md