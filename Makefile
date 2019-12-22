################################################################################
#
# Temporary makefile for jvxfsFramework library
#
# Copyright (C) 2019
#
#
# Contributor(s):
#
# Christian Thierfeld
#
################################################################################
CC = gcc
LD = gcc
MAJOR = 0
MINOR = 9
RELEASE = 0
NAME = jvxfs-framework
VERSION = $(MAJOR).$(MINOR).$(RELEASE)

EXE_WO = lib$(NAME).so
EXE_WP = $(EXE_WO).$(MAJOR)
EXE = $(EXE_WO).$(VERSION)

CFLAGS = -std=c99 -fPIC -Wall -g -I/usr/local/freeswitch/include/freeswitch/ -O0
CFLAGS += -DJVX_FS_FRAMEWORK_LIBVERSION="\"$(VERSION)\""
LDFLAGS = -shared -fPIC -Wl,-soname,$(EXE_WP)
LIBS = -L/usr/local/lib

ENGINE_LIBDIR = /usr/local/lib
ENGINE_INCDIR = /usr/local/include/jvxfs-framework

MODULES = . processing system utils
BUILD_DIR = OBJS
BUILD_SUBS = $(addprefix $(BUILD_DIR)/, $(MODULES))
BUILD_EXE = $(BUILD_DIR)/$(EXE)
INSTALL_INC_SUBS = $(addprefix $(ENGINE_INCDIR)/, $(MODULES))

SOURCES = $(foreach srcdir, $(MODULES), $(wildcard $(srcdir)/*.c))
HEADERS = $(foreach srcdir, $(MODULES), $(wildcard $(srcdir)/*.h))
OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))


vpath %.c $(MODULES)

define make-goal
$1/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $$@ -c $$<
endef

.PHONY: all rebuild checkdirs clean install uninstall

all: checkdirs $(BUILD_EXE)

rebuild: clean all

checkdirs: $(BUILD_SUBS)

$(BUILD_SUBS):
	@mkdir -pm 775 $@

clean:
	@rm -rf $(BUILD_DIR) *~ .libs/ core

$(foreach bdir, $(BUILD_SUBS), $(eval $(call make-goal, $(bdir))))

$(BUILD_EXE): $(OBJECTS)
	$(LD) $(LIBS) $(LDFLAGS) -o $@ $^

$(INSTALL_INC_SUBS):
	mkdir -pm 775 $@

install: $(BUILD_EXE) $(INSTALL_INC_SUBS)
	rm -rf $(ENGINE_LIBDIR)/$(EXE_WO)
	rm -rf $(ENGINE_LIBDIR)/$(EXE_WP)
	install $(BUILD_EXE) $(ENGINE_LIBDIR)
	ln -s $(ENGINE_LIBDIR)/$(EXE) $(ENGINE_LIBDIR)/$(EXE_WP)
	ln -s $(ENGINE_LIBDIR)/$(EXE) $(ENGINE_LIBDIR)/$(EXE_WO)
	$(foreach srcdir, $(MODULES), $(shell install $(filter-out $(wildcard $(srcdir)/*_private.h), $(wildcard $(srcdir)/*.h)) $(ENGINE_INCDIR)/$(srcdir)))

uninstall:
	rm -rf $(ENGINE_LIBDIR)/$(EXE_WO)*
	rm -rf $(ENGINE_INCDIR)
