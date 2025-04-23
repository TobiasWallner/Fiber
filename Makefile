.PHONY: deploy-docs
deploy-docs:
	doxygen
	git add docs --force
	git stash push
	git switch gh-pages
	git reset 9e8b3b7fb9eeb1ce85011073f4758e302230933c --hard
	git stash pop
	git commit -m "deploy-docs"
	git push origin gh-pages --force
	git switch main


.PHONY: build-test
build-test:
	cmake -S . -B build -G "Ninja Multi-Config" build -DEMBED_COMPILE_TESTS=ON -DEMBED_CTEST=ON
	cmake --build build --config Release

.PHONY: test
test: build-test
	ctest --test-dir build -C Release -V


.PHONY: test
test-debug:
	cmake -S . -B build -G "Ninja Multi-Config" -DEMBED_COMPILE_TESTS=ON
	cmake --build build --config Debug

.PHONY: clean
clean:
	cmake --build build --target clean