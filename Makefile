.PHONY: deploy-docs
deploy-docs:
	doxygen
	git stash push
	git add docs --force
	git stash push
	git switch gh-pages
	git reset 9e8b3b7fb9eeb1ce85011073f4758e302230933c --hard
	git stash pop
	git commit -m "deploy-docs"
	git push origin gh-pages --force
	git switch main
	git stash pop