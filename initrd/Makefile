.DEFAULT_GOAL := all

IMG := initrd

BUILD := build

BIN := cat help ls sh uname

.PHONY: all
all: $(BUILD)/$(IMG).tar

.PHONY: clean
clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).tar: $(patsubst %, $(BUILD)/rootfs/bin/%, $(BIN)) $(BUILD)/rootfs/etc
	tar -cvf - -C $(BUILD)/rootfs bin etc > $@

$(BUILD)/rootfs/bin/sh:
	@mkdir -p $(@D)
	curl -fSL -o $@ https://pkg.musl.cc/dash-0.5.10.2/aarch64-linux-musl/bin/dash

$(BUILD)/rootfs/bin/toybox:
	@mkdir -p $(@D)
	curl -fSL -o $@ https://pkg.musl.cc/toybox-0.8.3/aarch64-linux-musl/bin/toybox

$(BUILD)/rootfs/bin/%: $(BUILD)/rootfs/bin/toybox
	cp $< $@

$(BUILD)/rootfs/etc: etc
	cp -R $< $@
