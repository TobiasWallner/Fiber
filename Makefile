
.PHONY: deploy-docs
deploy-docs:
	doxygen .\Doxyfile
	git checkout gh-pages
	git add docs
	git commit -m "Deploy docs"
	git push origin gh-pages --force
	git switch main
