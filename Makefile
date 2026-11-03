Q ?= @
CC = arm-none-eabi-gcc
BUILD_DIR = output
BUILD_DIR_DEVICE = output/device

define object_for_dir
$(addprefix $(1)/,$(addsuffix .o,$(basename $(2))))
endef
NWLINK = npm_config_loglevel=silent npx --yes --quiet -- nwlink@1.0.0
LINK_GC = 1
LTO = 1
EXTERNAL_DATA ?= assets/input.bin

# Theme area written to the calculator: data/theme_area.bin, embedded as-is.
# THEMES_SIZE = bytes really used (trailing zero padding dropped, multiple of 4).
THEMES_BIN = data/theme_area.bin
THEMES_SIZE := $(shell python3 -c "d=open('$(THEMES_BIN)','rb').read();print((len(d.rstrip(b'\\0'))+3)//4*4)")

define object_for
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))
endef

src = $(addprefix src/,\
  app/display.c \
  app/timing.c \
  system/storage.c \
  system/flash.c \
  system/info.c \
  system/slots.c \
  themes/themes_data.c \
  dfu/dfu_slot.c \
  dfu/dfu_slot_data.c \
  main.c \
)

# 1 638 400
DFU_CAPACITY ?= 0x190000

CFLAGS_DEVICE = -std=c99
CFLAGS_DEVICE += -DDFU_CAPACITY=$(DFU_CAPACITY)
CFLAGS_DEVICE += $(shell $(NWLINK) eadk-cflags-device)
CFLAGS_DEVICE += -Os -Wall
CFLAGS_DEVICE += -ggdb
LDFLAGS = -Wl,--relocatable
LDFLAGS += -nostartfiles
LDFLAGS += --specs=nano.specs
# LDFLAGS += --specs=nosys.specs # Alternatively, use full-fledged newlib

ifeq ($(LINK_GC),1)
CFLAGS_DEVICE += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,-u,dfu_slot
endif

ifeq ($(LTO),1)
CFLAGS_DEVICE += -flto -fno-fat-lto-objects
CFLAGS_DEVICE += -fwhole-program
CFLAGS_DEVICE += -fvisibility=internal
LDFLAGS += -flinker-output=nolto-rel
endif

.PHONY: build
build: $(BUILD_DIR_DEVICE)/app.nwa

.PHONY: check
check: $(BUILD_DIR_DEVICE)/app.bin

.PHONY: run
run: build
	@echo "RUN"
	$(Q) if [ -s $(EXTERNAL_DATA) ]; then \
		$(NWLINK) install-nwa --external-data $(EXTERNAL_DATA) $(BUILD_DIR_DEVICE)/app.nwa; \
	else \
		$(NWLINK) install-nwa $(BUILD_DIR_DEVICE)/app.nwa; \
	fi

$(BUILD_DIR_DEVICE)/%.bin: $(BUILD_DIR_DEVICE)/%.nwa $(EXTERNAL_DATA)
	@echo "BIN     $@"
	$(Q) if [ -s $(EXTERNAL_DATA) ]; then \
		$(NWLINK) nwa-bin --external-data $(EXTERNAL_DATA) $< $@; \
	else \
		$(NWLINK) nwa-bin $< $@; \
	fi

$(BUILD_DIR_DEVICE)/%.elf: $(BUILD_DIR_DEVICE)/%.nwa $(EXTERNAL_DATA)
	@echo "ELF     $@"
	$(Q) if [ -s $(EXTERNAL_DATA) ]; then \
		$(NWLINK) nwa-elf --external-data $(EXTERNAL_DATA) $< $@; \
	else \
		$(NWLINK) nwa-elf $< $@; \
	fi

$(BUILD_DIR_DEVICE)/app.nwa: $(call object_for_dir,$(BUILD_DIR_DEVICE),$(src)) $(BUILD_DIR_DEVICE)/icon.o
	@echo "LD      $@"
	$(Q) $(CC) $(CFLAGS_DEVICE) $(LDFLAGS) $^ -o $@

$(BUILD_DIR_DEVICE)/src/storage/storage.o: src/storage/storage.c | $(BUILD_DIR_DEVICE)
	@echo "CC      $^"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) $(CFLAGS_DEVICE) -w -c $^ -o $@

# The theme area (.incbin of the .bin) is built without LTO and rebuilt when the .bin changes
%/themes_data.o: CFLAGS_DEVICE += -fno-lto -DTHEMES_SIZE=$(THEMES_SIZE)
$(BUILD_DIR_DEVICE)/src/themes/themes_data.o: $(THEMES_BIN)

# The slot is built without LTO so that it can be linked last (see dfu_slot_data.c)
%/dfu_slot_data.o: CFLAGS_DEVICE += -fno-lto

$(addprefix $(BUILD_DIR_DEVICE)/,%.o): %.c | $(BUILD_DIR_DEVICE)
	@echo "CC      $<"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) $(CFLAGS_DEVICE) -c $< -o $@

$(BUILD_DIR_DEVICE)/icon.o: assets/icon.png
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

.PRECIOUS: $(BUILD_DIR_DEVICE)
$(BUILD_DIR_DEVICE):
	$(Q) mkdir -p $@/src

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR_DEVICE)