BUILD_DIR=build
T3D_INST=$(shell realpath ../../tiny3d)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x

#https://github.com/N64brew-Game-Jam-2023/alien-sun/blob/main/Makefile

src = main.c

assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)



assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
			  $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm)))

all: startProg.z64

#https://github.com/breadbored/N64-RPG/blob/main/Makefile
# mksprite 16 2 2 assets/venusaur.png filesystem/venusaur.sprite

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o filesystem $@

$(BUILD_DIR)/startProg.dfs: $(assets_conv)
$(BUILD_DIR)/startProg.elf: $(src:%.c=$(BUILD_DIR)/%.o)

startProg.z64: N64_ROM_TITLE="Tiny3D - Model"
startProg.z64: $(BUILD_DIR)/startProg.dfs

clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean