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

config = Release
CC = gcc 
CXX = g++

.PHONY: build-test
build-test:
	cmake -S . -B build -G "Ninja Multi-Config" build -DFIBER_COMPILE_TESTS=ON -DFIBER_CTEST=ON -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX)
	cmake --build build --config $(config)

.PHONY: test
test: build-test
	ctest --test-dir build -C $(config) -V

.PHONY: clean
clean:
	cmake --build build --target clean