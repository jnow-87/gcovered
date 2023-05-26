#
# Copyright (C) 2020 Jan Nowotsch
# Author Jan Nowotsch	<jan.nowotsch@gmail.com>
#
# Released under the terms of the GNU GPL v2.0
#



# init build system
project_type := c
scripts_dir := scripts/build

# init config system
use_config_sys := n

# init source and build tree
ifneq ("$(DEBUG)","")
  default_build_tree := build/debug/
else
  default_build_tree := build/release/
endif

src_dirs := main/

# include build system Makefile
include $(scripts_dir)/main.make

# init default flags
cflags := $(CFLAGS) \
	-Wall \
	-Wextra \
	-Wshadow \
	-flto

cppflags := $(CPPFLAGS) \
	-Iinclude \
	-I$(build_tree)

ldflags := $(LDFLAGS)
ldlibs := $(LDLIOBSFLAGS)
asflags := $(ASFLAGS)
archflags := $(ARCHFLAGS)

yaccflags := $(YACCFLAGS)
lexflags := $(LEXFLAGS)
gperfflags := $(GPERFFLAGS)


####
## targets
####

## build
.PHONY: all
ifneq ("$(DEBUG)","")
all: cflags += -g
all: cxxflags += -g
all: asflags += -g
all: ldlibs += -g
endif

all: $(lib) $(bin)

## cleanup
.PHONY: clean
clean:
	$(rm) $(filter-out $(build_tree)/$(scripts_dir),$(wildcard $(build_tree)/*))

.PHONY: distclean
distclean:
	$(rm) $(build_tree)

## install
include $(scripts_dir)/install.make

.PHONY: install
install: all
	$(call install,$(bin))

.PHONY: uninstall
uninstall:
	$(call uninstall,$(addprefix $(PREFIX)/,$(notdir $(bin))))
