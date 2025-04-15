
.PHONY: deploy-docs
deploy-docs:
	doxygen .\Doxyfile
	git checkout gh-pages
ifeq ($(OS),Windows_NT)
	if exist page rmdir /S /Q page
else
	[ -d page ] && rm -rf page
endif
	mkdir page
ifeq ($(OS),Windows_NT)
	robocopy docs\html page /E /NFL /NDL /NJH /NJS /nc /ns /np
else
	cp -r docs/html/* page/
endif
	git add page
	git commit -m "Deploy docs"
	git push origin gh-pages --force
	git switch main
