TARGET := dtest
DEPS := $(TARGET).bib fig/overview.pdf

HTML_DIR := html
PDF_DIR := pdf
MADOKO := madoko

MARKDOWN := $(TARGET).md
PDF := $(PDF_DIR)/$(TARGET).pdf
HTML := $(HTML_DIR)/$(TARGET).html

# Shortcuts.
.PHONY: pdf html
pdf: $(PDF)
html: $(HTML)

# Build PDF via LaTeX.
$(PDF): $(MARKDOWN) $(DEPS)
	$(MADOKO) --odir=$(PDF_DIR) --pdf $<

# Build Web page.
$(HTML): $(MARKDOWN) $(DEPS)
	$(MADOKO) --odir=$(HTML_DIR) $<

.PHONY: clean
clean:
	rm -rf $(PDF_DIR) $(HTML_DIR)

.PHONY: deploy
DEST_PATH := adriansampson.net/drafts
DEST := dh:domains/$(DEST_PATH)
DEST_URL := http://$(DEST_PATH)/$(notdir $(PDF))
deploy: $(PDF)
	scp $< $(DEST)
	@echo $(DEST_URL)


# View products.

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
OPEN ?= open
else
OPEN ?= xdg-open
endif

.PHONY: view view-html
view: $(PDF)
	$(OPEN) $(PDF)
view-html: $(HTML)
	$(OPEN) $(HTML)

# Auto-build based on `livereload`.
.PHONY: watch
define WATCH_PY
endef
export WATCH_PY
watch:
	python watch.py $(MARKDOWN) $(DEPS)
