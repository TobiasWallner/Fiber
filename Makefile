
.PHONY: deploy-docs
deploy-docs:
	doxygen .\Doxyfile
	git add docs -f
	git stash -m "deploy docs"
	git checkout gh-pages
	git reset 2c3613bd379ff06e0b39626a3e53479a2b032e6c --hard
	git stash apply
	git commit -m "Deploy docs"
	git push origin gh-pages --force
	git switch main --force
	git stash drop 'stash@{0}'
