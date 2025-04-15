
.PHONY: deploy-docs
deploy-docs:
	doxygen .\Doxyfile
	git add docs -f
	git stash -m "deploy docs"
	git checkout gh-pages
	git add docs
	git commit -m "Deploy docs"
	git push origin gh-pages
	git switch main
