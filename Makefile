SHELL := /bin/bash

CLEAR_DIR = find . -mindepth 1 -not -name '.git' -exec rm -rf {} +

COPY_DOCS = cp -r ../docs/* .

.PHONY: deploy-docs
deploy-docs:
	doxygen Doxyfile
	git checkout gh-pages
	$(CLEAR_DIR)
	$(COPY_DOCS)
	git add -A
	git commit -m "Update docs"
	# git push
	git checkout main
