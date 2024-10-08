.PHONY: clean run format test

BINARY_NAME := instablock

CC := gcc
SRC_DIR := src/main
TEST_DIR := src/test
BUILD_DIR := out
OBJ_DIR := $(BUILD_DIR)/obj
DEPS_DIR := $(BUILD_DIR)/deps
TEST_OBJ_DIR := $(BUILD_DIR)/test

SOURCES := $(wildcard $(SRC_DIR)/*/c/*.c)
OBJECTS := $(subst $(SRC_DIR)/,$(OBJ_DIR)/,$(patsubst %.c,%.o,$(SOURCES)))
DEPENDS := $(subst $(SRC_DIR)/,$(DEPS_DIR)/,$(patsubst %.c,%.d,$(SOURCES)))
INCLUDE := $(addprefix -I,$(wildcard $(SRC_DIR)/*/header))

TEST_SOURCES := $(wildcard $(TEST_DIR)/*/*.c)
TEST_OBJECTS := $(subst $(TEST_DIR)/,$(TEST_OBJ_DIR)/,$(patsubst %.c,%.o,$(TEST_SOURCES)))
SOURCES_WITHOUT_MAIN := $(filter-out %/main.c,$(SOURCES))

CFLAGS := $(INCLUDE) -std=c99 -W -Wall -Wextra -pedantic -pedantic-errors -Wstrict-prototypes # -Wdeclaration-after-statement
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

$(TEST_OBJ_DIR)/test_runner.o $(TEST_OBJECTS): $(TEST_SOURCES) $(TEST_DIR)/test_runner.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(subst $(TEST_OBJ_DIR)/,$(TEST_DIR)/,$(patsubst %.o,%.c,$@)) $(SOURCES_WITHOUT_MAIN) $(CFLAGS) $(LINKER_ARGS)

test: $(TEST_OBJ_DIR)/test_runner.o $(TEST_OBJECTS)
	$(TEST_OBJ_DIR)/test_runner.o $(TEST_OBJECTS)
