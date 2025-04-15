# Detect platform
UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
	# Native Windows (e.g., CMD or PowerShell)
	RM = del
	COPY = copy
else ifeq ($(UNAME_S),Linux)
	RM = rm -rf
	COPY = cp -r
else ifeq ($(UNAME_S),Darwin)
	RM = rm -rf
	COPY = cp -r
else
	# Fallback
	RM = rm -rf
	COPY = cp -r
endif

.PHONY: deploy-docs
deploy-docs:
	doxygen Doxyfile
	git checkout gh-pages
	git rm -rf *
	COPY ../docs/* .
	git add -A
	git commit -m "Update docs"
	# git push
	git checkout main
