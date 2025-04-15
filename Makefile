
.PHONY: deploy-docs
deploy-docs:
	doxygen Doxyfile
	git checkout gh-pages
	git rm -rf .
	git add -A
	git commit -m "Update docs"
	git checkout main
