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

all: $(BUILD)/deadgl $(BUILD)/deadgl-inspect $(BUILD)/libdeadgl.a

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/deadgl.o: src/deadgl.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadgl_cli.o: src/deadgl_cli.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Dmain=deadgl_render_main -c $< -o $@

$(BUILD)/deadgl_front.o: src/deadgl_front.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadgl_inspect_core.o: src/deadgl_inspect_core.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadgl_inspect.o: src/deadgl_inspect.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/test_deadgl.o: tests/test_deadgl.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/libdeadgl.a: $(BUILD)/deadgl.o
	$(AR) rcs $@ $^

$(BUILD)/deadgl: $(BUILD)/deadgl.o $(BUILD)/deadgl_cli.o $(BUILD)/deadgl_front.o $(BUILD)/deadgl_inspect_core.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/deadgl-inspect: $(BUILD)/deadgl.o $(BUILD)/deadgl_inspect.o $(BUILD)/deadgl_inspect_core.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/test_deadgl: $(BUILD)/deadgl.o $(BUILD)/test_deadgl.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

test: $(BUILD)/deadgl $(BUILD)/deadgl-inspect $(BUILD)/test_deadgl
	$(BUILD)/test_deadgl
	$(BUILD)/deadgl --version
	$(BUILD)/deadgl inspect examples/near_clip.dgl > $(BUILD)/near_clip.main.inspect
	$(BUILD)/deadgl audit examples/command_machine.dgl > $(BUILD)/command_machine.audit
	$(BUILD)/deadgl pack examples/near_clip.dgl -o $(BUILD)/near_clip.dgb
	$(BUILD)/deadgl disasm $(BUILD)/near_clip.dgb > $(BUILD)/near_clip.disasm
	$(BUILD)/deadgl unpack $(BUILD)/near_clip.dgb -o $(BUILD)/near_clip.unpack.dgl
	cmp examples/near_clip.dgl $(BUILD)/near_clip.unpack.dgl
	$(BUILD)/deadgl-inspect --version
	$(BUILD)/deadgl demo shrine -o $(BUILD)/shrine.ppm
	$(BUILD)/deadgl demo depth -o $(BUILD)/depth.ppm
	$(BUILD)/deadgl demo cube -o $(BUILD)/cube.ppm
	$(BUILD)/deadgl run examples/shrine.dgl -o $(BUILD)/scene.ppm
	$(BUILD)/deadgl run examples/command_machine.dgl -o $(BUILD)/command_machine.ppm
	$(BUILD)/deadgl prove examples/near_clip.dgl -o $(BUILD)/near_clip.ppm -p $(BUILD)/near_clip.proof
	$(BUILD)/deadgl-inspect examples/near_clip.dgl > $(BUILD)/near_clip.inspect
	$(BUILD)/deadgl hash examples/near_clip.dgl
	test -s $(BUILD)/near_clip.main.inspect
	test -s $(BUILD)/command_machine.audit
	test -s $(BUILD)/near_clip.dgb
	test -s $(BUILD)/near_clip.disasm
	test -s $(BUILD)/near_clip.unpack.dgl
	test -s $(BUILD)/near_clip.inspect
	test -s $(BUILD)/near_clip.proof

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
	$(BUILD)/deadgl demo cube -o $(BUILD)/cube.ppm
	$(BUILD)/deadgl run examples/near_clip.dgl -o $(BUILD)/near_clip.ppm

debug:
	sh scripts/debug_probe.sh

bench:
	sh scripts/bench.sh examples/command_machine.dgl

release-local:
	sh scripts/release.sh

clean:
	rm -rf $(BUILD) build-cmake deadgl_test.ppm
