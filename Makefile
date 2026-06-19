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

all: $(BUILD)/deadgl $(BUILD)/deadgl-inspect $(BUILD)/deadpad $(BUILD)/deadview $(BUILD)/libdeadgl.a

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/deadgl.o: src/deadgl.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadgl_tile.o: src/deadgl_tile.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadgl_cli.o: src/deadgl_cli.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Dmain=deadgl_render_main -c $< -o $@

$(BUILD)/deadgl_front.o: src/deadgl_front.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadgl_inspect_core.o: src/deadgl_inspect_core.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadgl_inspect.o: src/deadgl_inspect.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadpad.o: src/deadpad.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/deadview.o: src/deadview.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/test_deadgl.o: tests/test_deadgl.c include/deadgl.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/libdeadgl.a: $(BUILD)/deadgl.o $(BUILD)/deadgl_tile.o
	$(AR) rcs $@ $^

$(BUILD)/deadgl: $(BUILD)/deadgl.o $(BUILD)/deadgl_tile.o $(BUILD)/deadgl_cli.o $(BUILD)/deadgl_front.o $(BUILD)/deadgl_inspect_core.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/deadgl-inspect: $(BUILD)/deadgl.o $(BUILD)/deadgl_inspect.o $(BUILD)/deadgl_inspect_core.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/deadpad: $(BUILD)/deadgl.o $(BUILD)/deadpad.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/deadview: $(BUILD)/deadgl.o $(BUILD)/deadview.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/test_deadgl: $(BUILD)/deadgl.o $(BUILD)/deadgl_tile.o $(BUILD)/test_deadgl.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

test: $(BUILD)/deadgl $(BUILD)/deadgl-inspect $(BUILD)/deadpad $(BUILD)/deadview $(BUILD)/test_deadgl
	$(BUILD)/test_deadgl
	$(BUILD)/deadgl --version
	$(BUILD)/deadpad --version
	$(BUILD)/deadview --version
	$(BUILD)/deadpad new $(BUILD)/deadpad_seed.dgl
	$(BUILD)/deadpad append $(BUILD)/deadpad_seed.dgl line 8 8 120 8 0xff8822
	$(BUILD)/deadpad cat $(BUILD)/deadpad_seed.dgl > $(BUILD)/deadpad_seed.cat
	$(BUILD)/deadgl run $(BUILD)/deadpad_seed.dgl -o $(BUILD)/deadpad_seed.ppm
	printf 'canvas 64 64\nclear #050608\nline 4 4 60 60 #00ff99\n' | $(BUILD)/deadgl shell -o $(BUILD)/shell.ppm
	$(BUILD)/deadgl shell $(BUILD)/deadpad_seed.dgl -o $(BUILD)/shell_file.ppm
	$(BUILD)/deadgl textdemo -o $(BUILD)/textdemo.ppm
	$(BUILD)/deadgl tiledemo -o $(BUILD)/tiledemo.ppm
	$(BUILD)/deadview $(BUILD)/tiledemo.ppm > $(BUILD)/tiledemo.view
	$(BUILD)/deadgl scenepack examples/command_machine.dgl -o $(BUILD)/command_machine.dgp
	$(BUILD)/deadgl sceneunpack $(BUILD)/command_machine.dgp -o $(BUILD)/command_machine.unpack.dgl
	cmp examples/command_machine.dgl $(BUILD)/command_machine.unpack.dgl
	$(BUILD)/deadgl run $(BUILD)/command_machine.unpack.dgl -o $(BUILD)/command_machine.unpack.ppm
	$(BUILD)/deadgl suite examples/command_machine.dgl -o $(BUILD)/command_machine.suite
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
	test -s $(BUILD)/deadpad_seed.dgl
	test -s $(BUILD)/deadpad_seed.cat
	test -s $(BUILD)/deadpad_seed.ppm
	test -s $(BUILD)/shell.ppm
	test -s $(BUILD)/shell_file.ppm
	test -s $(BUILD)/textdemo.ppm
	test -s $(BUILD)/tiledemo.ppm
	test -s $(BUILD)/tiledemo.view
	test -s $(BUILD)/command_machine.dgp
	test -s $(BUILD)/command_machine.unpack.dgl
	test -s $(BUILD)/command_machine.unpack.ppm
	test -s $(BUILD)/command_machine.suite
	test -s $(BUILD)/command_machine.suite.ppm
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

demo: $(BUILD)/deadgl $(BUILD)/deadpad $(BUILD)/deadview
	$(BUILD)/deadgl demo shrine -o $(BUILD)/shrine.ppm
	$(BUILD)/deadgl demo depth -o $(BUILD)/depth.ppm
	$(BUILD)/deadgl demo cube -o $(BUILD)/cube.ppm
	$(BUILD)/deadgl textdemo -o $(BUILD)/textdemo.ppm
	$(BUILD)/deadgl tiledemo -o $(BUILD)/tiledemo.ppm
	$(BUILD)/deadview $(BUILD)/tiledemo.ppm > $(BUILD)/tiledemo.view
	$(BUILD)/deadpad new $(BUILD)/deadpad_seed.dgl
	$(BUILD)/deadgl shell $(BUILD)/deadpad_seed.dgl -o $(BUILD)/shell_file.ppm
	$(BUILD)/deadgl scenepack examples/command_machine.dgl -o $(BUILD)/command_machine.dgp
	$(BUILD)/deadgl sceneunpack $(BUILD)/command_machine.dgp -o $(BUILD)/command_machine.unpack.dgl
	$(BUILD)/deadgl suite examples/command_machine.dgl -o $(BUILD)/command_machine.suite
	$(BUILD)/deadgl run $(BUILD)/deadpad_seed.dgl -o $(BUILD)/deadpad_seed.ppm
	$(BUILD)/deadgl run examples/near_clip.dgl -o $(BUILD)/near_clip.ppm

debug:
	sh scripts/debug_probe.sh

bench:
	sh scripts/bench.sh examples/command_machine.dgl

release-local:
	sh scripts/release.sh

clean:
	rm -rf $(BUILD) build-cmake deadgl_test.ppm
