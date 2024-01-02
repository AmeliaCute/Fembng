CC=gcc
CFLAGS=-Wall -Wextra -std=c11

SRC_DIR=source
BUILD_DIR=build
BIN_DIR=bin

COMMON_DIR=$(BUILD_DIR)/common
FEMBNG_DIR=$(BUILD_DIR)/fembng
FEMBNGAPP_DIR=$(BUILD_DIR)/fembngapp

# Create build directories if they don't exist
$(shell mkdir -p $(BIN_DIR) $(COMMON_DIR) $(FEMBNG_DIR) $(FEMBNGAPP_DIR))

# Common source files
SRCS_COMMON=$(wildcard $(SRC_DIR)/common/*.c)
OBJS_COMMON=$(patsubst $(SRC_DIR)/common/%.c, $(COMMON_DIR)/%.o, $(SRCS_COMMON))

# Source files for fembng
SRCS_FEMBNG=$(wildcard $(SRC_DIR)/fembng/*.c)
OBJS_FEMBNG=$(patsubst $(SRC_DIR)/fembng/%.c, $(FEMBNG_DIR)/%.o, $(SRCS_FEMBNG))
TARGET_FEMBNG=$(BIN_DIR)/fembng

# Source files for fembngapp
SRCS_FEMBNGAPP=$(wildcard $(SRC_DIR)/fembngapp/*.c)
OBJS_FEMBNGAPP=$(patsubst $(SRC_DIR)/fembngapp/%.c, $(FEMBNGAPP_DIR)/%.o, $(SRCS_FEMBNGAPP))
TARGET_FEMBNGAPP=$(BIN_DIR)/fembngapp

all: $(TARGET_FEMBNG) $(TARGET_FEMBNGAPP)

# Compile common source files
$(COMMON_DIR)/%.o: $(SRC_DIR)/common/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile fembng
$(TARGET_FEMBNG): $(OBJS_COMMON) $(OBJS_FEMBNG)
	$(CC) $(CFLAGS) -o $@ $^

$(FEMBNG_DIR)/%.o: $(SRC_DIR)/fembng/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile fembngapp
$(TARGET_FEMBNGAPP): $(OBJS_COMMON) $(OBJS_FEMBNGAPP)
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

$(FEMBNGAPP_DIR)/%.o: $(SRC_DIR)/fembngapp/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/common/*.o $(BUILD_DIR)/fembng/*.o $(BUILD_DIR)/fembngapp/*.o $(TARGET_FEMBNG) $(TARGET_FEMBNGAPP)

test: all
	./$(TARGET_FEMBNG) -w 5000 -h 5000 -o test1
	./$(TARGET_FEMBNGAPP) ./test1.femboy

.PHONY: all clean
