CC ?= cc
AR ?= ar
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Werror
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
LDLIBS ?= -lm
BUILD := build
SAN_CFLAGS := -std=c99 -O1 -g -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Werror -fsanitize=address,undefined -fno-omit-frame-pointer
SAN_LDFLAGS := -fsanitize=address,undefined

.PHONY: all clean test sanitize demo debug bench release-local

all: $(BUILD)/deadgl $(BUILD)/libdeadgl.a

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/deadgl.o: src/deadgl.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadgl_cli.o: src/deadgl_cli.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/test_deadgl.o: tests/test_deadgl.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/libdeadgl.a: $(BUILD)/deadgl.o
	$(AR) rcs $@ $^

$(BUILD)/deadgl: $(BUILD)/deadgl.o $(BUILD)/deadgl_cli.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/test_deadgl: $(BUILD)/deadgl.o $(BUILD)/test_deadgl.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

test: $(BUILD)/deadgl $(BUILD)/test_deadgl
	$(BUILD)/test_deadgl
	$(BUILD)/deadgl --version
	$(BUILD)/deadgl demo shrine -o $(BUILD)/shrine.ppm
	$(BUILD)/deadgl demo depth -o $(BUILD)/depth.ppm
	$(BUILD)/deadgl run examples/shrine.dgl -o $(BUILD)/scene.ppm
	$(BUILD)/deadgl prove examples/shrine.dgl -o $(BUILD)/proof.ppm -p $(BUILD)/proof.txt
	$(BUILD)/deadgl hash examples/shrine.dgl
	test -s $(BUILD)/shrine.ppm
	test -s $(BUILD)/depth.ppm
	test -s $(BUILD)/scene.ppm
	test -s $(BUILD)/proof.ppm
	test -s $(BUILD)/proof.txt

sanitize:
	$(MAKE) clean
	mkdir -p $(BUILD)
	printf 'int main(void){return 0;}\n' > $(BUILD)/san_check.c
	@if $(CC) $(SAN_CFLAGS) $(BUILD)/san_check.c $(SAN_LDFLAGS) -o $(BUILD)/san_check >/dev/null 2>&1; then \
		$(MAKE) test CFLAGS="$(SAN_CFLAGS)" LDFLAGS="$(SAN_LDFLAGS)"; \
	else \
		echo "SKIP sanitize: sanitizer runtime unavailable for CC=$(CC)"; \
	fi
	$(MAKE) clean

demo: $(BUILD)/deadgl
	$(BUILD)/deadgl demo shrine -o $(BUILD)/shrine.ppm
	$(BUILD)/deadgl demo depth -o $(BUILD)/depth.ppm

debug:
	sh scripts/debug_probe.sh

bench:
	sh scripts/bench.sh examples/command_machine.dgl

release-local:
	sh scripts/release.sh

clean:
	rm -rf $(BUILD) build-cmake deadgl_test.ppm
