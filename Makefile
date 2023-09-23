.DEFAULT_GOAL := help

define HELP
print('\nUsage: make \033[92m[target]\033[0m\n')
with open('Makefile') as f:
	for line in f.readlines():
		if 'elif' in line or 'print' in line: continue
		if line.startswith('#>'):
			print(line.strip()[3:])
		elif ': ##> ' in line:
			print('    \033[92m{}\033[0m - {}'.format(*line.strip().split(': ##> ')))
print('')
endef
export HELP

.PHONY: help
help: ##> print the usage message
	@python3 -c "$$HELP"

.PHONY: build
build: ##> builds the project
	mkdir -p build
	(cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make)

.PHONY: run
run: ##> runs the project
	./build/ExampleApp

.PHONY: clean
clean: ##> removes the build files
	mkdir -p build
	(cd build && make clean)

.PHONY: test
test: ##> runs the unit tests
	./build/bin/unit_tests --gtest_shuffle

.PHONY: format
format: ##> formats the code
	find include test -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i

.PHONY: docker-run
docker-run: ##> pulls and runs the docker container for dev and test
	docker pull ghcr.io/nrockwood/devcontainer:latest
	docker run --rm -it -w /workspace -v $(shell pwd):/workspace ghcr.io/nrockwood/devcontainer /bin/zsh
