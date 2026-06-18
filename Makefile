CC ?= cc
AR ?= ar
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Werror
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
LDLIBS ?= -lm
BUILD := build

.PHONY: all clean test sanitize demo

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
	$(BUILD)/deadgl hash examples/shrine.dgl
	test -s $(BUILD)/shrine.ppm
	test -s $(BUILD)/depth.ppm
	test -s $(BUILD)/scene.ppm

sanitize:
	$(MAKE) clean
	$(MAKE) test CFLAGS="-std=c99 -O1 -g -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Werror -fsanitize=address,undefined -fno-omit-frame-pointer" LDFLAGS="-fsanitize=address,undefined"

demo: $(BUILD)/deadgl
	$(BUILD)/deadgl demo shrine -o $(BUILD)/shrine.ppm
	$(BUILD)/deadgl demo depth -o $(BUILD)/depth.ppm

clean:
	rm -rf $(BUILD) build-cmake deadgl_test.ppm
