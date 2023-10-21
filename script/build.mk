.DEFUALT_GOAL := app

SHARE = 1
ifeq ($(SHARE),1)
SO = -so
CFLAGS  += -fPIC -fvisibility=hidden
LDFLAGS += -shared -fPIC
else 
SO =
LIBS += -lSDL2 -ldl -pie
endif

CC = gcc
CXX = g++
LD = $(CXX)

WORK_DIR = $(shell pwd)
BUILD_DIR = $(WORK_DIR)/build
TAR_DIR = $(BUILD_DIR)/obj

BINARY = $(BUILD_DIR)/simulator$(SO)

INC_PATH = $(WORK_DIR)/include
INCLUDE = $(addprefix -I, $(INC_PATH))

CFLAGS := -O3 -MMD -Wall -Werror $(INCLUDE) $(CFLAGS)
LDFLAGS := -O3 $(LDFLAGS)

OBJS = $(addprefix $(TAR_DIR)/, $(addsuffix .o, $(basename $(SRCS))))


$(TAR_DIR)/%.o: %.c
	@mkdir -p $(dir $@) && echo "$(COLOR_DBLUE)[CC]$(COLOR_NONE) $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(TAR_DIR)/%.o: %.cc
	@mkdir -p $(dir $@) && echo "$(COLOR_DBLUE)[CXX]$(COLOR_NONE) $<"
	@$(CXX) $(CFLAGS) -c $< -o $@

app: $(BINARY)

$(BINARY): $(OBJS) 
	@mkdir -p $(dir $@) && echo "$(COLOR_YELLOW)[LD]$(COLOR_NONE) build/$(notdir $@)"
	@echo $(LD) $(LDFLAGS)  $^ -o $@ $(LIBS)
	@$(LD) $(LDFLAGS)  $^ -o $@

ARGS = 
run: $(BINARY)
	@echo "$(COLOR_YELLOW)[RUN]$(COLOR_NONE) build/$(notdir $<)"
	@$(BINARY) $(IMG) $(ARGS)

gdb: $(BINARY)
	@echo "$(COLOR_YELLOW)[GDB]$(COLOR_NONE) build/$(notdir $<)"
	@gdb $(SIM_PATH) --args $(BINARY) $(IMG)

clean:
	rm -rf $(BUILD_DIR)


