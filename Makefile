.PHONY: clean run format

CC := gcc
SRC_DIR := src
BUILD_DIR := out
OBJ_DIR := $(BUILD_DIR)/obj
DEPS_DIR := $(BUILD_DIR)/deps
BINARY_NAME := instablock

SOURCES := $(wildcard $(SRC_DIR)/*/c/*.c)
OBJECTS := $(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(patsubst %.c,%.o,$(SOURCES)))
DEPENDS := $(subst $(SRC_DIR)/,$(DEPS_DIR)/,$(patsubst %.c,%.d,$(SOURCES)))
INCLUDE := $(addprefix -I,$(wildcard $(SRC_DIR)/*/header))

CFLAGS := $(INCLUDE) -std=c99 -W -Wall -Wextra -pedantic -pedantic-errors -Wstrict-prototypes -Wdeclaration-after-statement
LINKER_ARGS := -lglfw -lGLEW -framework OpenGL

$(BUILD_DIR)/$(BINARY_NAME): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(CFLAGS) $(LINKER_ARGS)

# create template for %.d files
define deps_template
TARG := $$(subst $$(SRC_DIR)/,$$(DEPS_DIR)/,$$(patsubst %.c,%.d,$(1)))
$$(TARG): $(1)
	@mkdir -p $$(@D)
	$$(CC) $$(CFLAGS) -MF"$$@" -MG -MM -MT"$$@" -MT"$$(subst $$(SRC_DIR)/,$$(OBJ_DIR)/,$$(patsubst %.c,%.o,$$<))" "$$<"
endef
$(foreach SOURCE,$(SOURCES),$(eval $(call deps_template,$(SOURCE))))
include $(DEPENDS)

# create template for %.o files
define obj_template
TARG := $$(subst $$(SRC_DIR)/,$$(OBJ_DIR)/,$$(patsubst %.c,%.o,$(1)))
$$(TARG): $(1)
	@mkdir -p $$(@D)
	$$(CC) -c -o $$@ $$< $$(CFLAGS)
endef
$(foreach SOURCE,$(SOURCES),$(eval $(call obj_template,$(SOURCE))))

format:
	find src -type f \( -name '*.c' -o -name '*.h' \) | xargs clang-format -i

clean:
	rm -r $(BUILD_DIR)

run: $(BUILD_DIR)/$(BINARY_NAME)
	$(BUILD_DIR)/$(BINARY_NAME)
