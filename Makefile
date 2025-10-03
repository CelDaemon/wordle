NAME = wordle
LDLIBS = -lz

SRCS = wordle.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

BUILD_DIR ?= build

.PHONY: all clean
all: $(BUILD_DIR)/$(NAME) $(BUILD_DIR)/words.db.gz

$(BUILD_DIR)/words.db: words.txt
	tr -d '\n' < $< > $@

$(BUILD_DIR)/words.db.gz: $(BUILD_DIR)/words.db
	gzip -c $< > $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^ $(OUTPUT_OPTION)

$(BUILD_DIR)/$(NAME): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) $(OUTPUT_OPTION)

clean:
	$(RM) $(BUILD_DIR)/*

run: all
	cd $(BUILD_DIR) && ./$(NAME)
