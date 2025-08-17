#####################################
# Southern New Hampshire University #
# CS-300 Portfolio Project          #
#                                   #
# Student - Nathan Fisher           #
# Date - 08/15/2025                 #
#####################################

BIN     = ProjectTwo
SRC     = $(BIN).cpp
DOC_SRC = runtime_analysis.md
HTMLDOC = $(patsubst %.md,%.html, $(DOC_SRC))
PDFDOC  = $(patsubst %.md,%.pdf, $(DOC_SRC))
MSDOC   = $(patsubst %.md,%.docx, $(DOC_SRC))
DOCS   += $(HTMLDOC)
DOCS   += $(PDFDOC)
DOCS   += $(MSDOC)

OBJS   += $(BIN)
OBJS   += $(DOCS)

bin: $(BIN)

all: $(OBJS)

docs: $(DOCS)

html: $(HTMLDOC)

pdf: $(PDFDOC)

docx: $(MSDOC)

$(BIN): $(SRC)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(HTMLDOC): $(DOC_SRC)
	pandoc --standalone $< -o $@

$(PDFDOC): $(DOC_SRC)
	pandoc -t latex $< -o $@

$(MSDOC): $(DOC_SRC) reference-doc.docx template.openxml
	pandoc --reference-doc=reference-doc.docx --template=template.openxml $< -o $@

clean:
	rm -rf $(OBJS)

.PHONY: bin all docs pdf html docx clean
