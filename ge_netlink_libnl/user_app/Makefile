# tool macros
CC := gcc
_CFLAGS := $(CFLAGS)
_LDFLAGS := $(LDFLAGS) -lnl-3 -lnl-genl-3

SRC_PATH := src
INC_PATH := -I../include/libnl3

# path macros
BIN_PATH := bin
OBJ_PATH := obj

# compile macros
TARGET_NAME := ge_netlink_user
TARGET := $(BIN_PATH)/$(TARGET_NAME)

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(x)/*.c))
OBJ := ${SRC:%.c=${OBJ_PATH}/%.o}

# clean files list
DISTCLEAN_LIST := $(OBJ)
CLEAN_LIST := $(TARGET) \
			  $(DISTCLEAN_LIST)

# default rule
default: makedir all

# non-phony targets
$(TARGET): $(OBJ)
	@echo "Info: all objs => $@"
	$(CC) -o $@ $^ $(_LDFLAGS)

$(OBJ_PATH)/%.o:%.c
	@echo "Info: $^ => $@"
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(_CFLAGS) $(INC_PATH) -c $< -o $@

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf $(OBJ_PATH)
	rm -rf $(BIN_PATH)

.PHONY: print
print:
	echo $(OBJ)
