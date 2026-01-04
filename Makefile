# ------------------------
# Projet
# ------------------------
PROJECT_NAME := bottom

CC := gcc
GDB := gdb

SRC_DIR := sources
UNIT_TEST_DIR := unit-test
BUILD_DIR := build
BUILD_UT_DIR := build-unit-test

FLAGS := -Wall -Wextra -Werror -Wpedantic -Werror=unused-result -Wunused-function -std=gnu23 -D_GNU_SOURCE -lncursesw -lssh -lpthread
DEBUG_FLAGS := -g -Og
RELEASE_FLAGS := -DNDEBUG -O2

# ------------------------
# Fichiers sources
# ------------------------
SRC_FILES := $(shell find $(SRC_DIR) -type f -name "*.c")
HEADERS := $(shell find $(SRC_DIR) -type f -name "*.h")
C_FILES := $(filter $(SRC_FILES), $(HEADERS:.h=.c))

INCLUDE_DIRS := $(addprefix -I, $(shell find $(SRC_DIR) -type d))
UNIT_TEST_FILES := $(shell find $(UNIT_TEST_DIR) -type f -name "*.c")

MAKEFLAGS += --no-print-directory

# ------------------------
# Cibles phony
# ------------------------
.PHONY: all debug release run run-debug run-all-unit-tests clean clean-unit-test

# ------------------------
# Cible par défaut
# ------------------------
all: release

# ------------------------
# Debug / Release
# ------------------------
debug: | $(BUILD_DIR)
	$(CC) $(DEBUG_FLAGS) $(INCLUDE_DIRS) $(SRC_FILES) -o $(BUILD_DIR)/$(PROJECT_NAME) $(FLAGS)

release: | $(BUILD_DIR)
	$(CC) $(RELEASE_FLAGS) $(INCLUDE_DIRS) $(SRC_FILES) -o $(BUILD_DIR)/$(PROJECT_NAME) $(FLAGS)

# ------------------------
# Tests unitaires
# ------------------------
$(BUILD_UT_DIR)/%: $(UNIT_TEST_DIR)/%.c $(SRC_FILES) | $(BUILD_UT_DIR)
	$(CC) $(DEBUG_FLAGS) $(INCLUDE_DIRS) $^ -o $@ $(FLAGS)

# Exécution de tous les tests
run-all-unit-tests: $(UNIT_TEST_FILES:$(UNIT_TEST_DIR)/%.c=$(BUILD_UT_DIR)/%)
	@for exe in $^; do [ -x $$exe ] && ./$$exe; done

# ------------------------
# Exécution du binaire principal
# ------------------------
run: release
	./$(BUILD_DIR)/$(PROJECT_NAME)

run-debug: debug
	$(GDB) ./$(BUILD_DIR)/$(PROJECT_NAME)

# ------------------------
# Nettoyage
# ------------------------
clean:
	rm -rf $(BUILD_DIR)

clean-unit-test:
	rm -rf $(BUILD_UT_DIR)

# ------------------------
# Création des répertoires
# ------------------------
$(BUILD_DIR) $(BUILD_UT_DIR):
	mkdir -p $@

