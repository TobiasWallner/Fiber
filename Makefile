.PHONY: deploy-docs
deploy-docs:
	doxygen
	git add docs --force
	git add extern/doxygen-awesome-css --force
	git stash commit -m "deploy docs"
	git checkout gh-pages
	git reset --hard
