BUILD_DIR=build
include $(N64_INST)/include/n64.mk

src = spritetest.c
assets_png = $(wildcard assets/*.png)

assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite)))


AUDIOCONV_FLAGS ?=
MKSPRITE_FLAGS ?=

all: spritetest.z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

$(BUILD_DIR)/spritetest.dfs: $(assets_conv)
$(BUILD_DIR)/spritetest.elf: $(src:%.c=$(BUILD_DIR)/%.o)

spritetest.z64: N64_ROM_TITLE="Sprite Anim Demo"
spritetest.z64: $(BUILD_DIR)/spritetest.dfs

clean:
	rm -rf $(BUILD_DIR) spritetest.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
