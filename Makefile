SRC_DIR = libfalcon

SRC = $(SRC_DIR)/falcon.c $(SRC_DIR)/codec.c $(SRC_DIR)/common.c\
      $(SRC_DIR)/fft.c $(SRC_DIR)/fpr.c $(SRC_DIR)/keygen.c\
			$(SRC_DIR)/rng.c $(SRC_DIR)/shake.c $(SRC_DIR)/sign.c\
			$(SRC_DIR)/speed.c $(SRC_DIR)/vrfy.c\
      c_src/falcon_nif.c

ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS += -Wall -Wextra -Wshadow -Wundef -O3 -g -I$(SRC_DIR) -Ic_src -I$(ERLANG_PATH)

KERNEL_NAME := $(shell uname -s)

LIB_NAME = $(MIX_APP_PATH)/priv/falcon_nif.so
ifneq ($(CROSSCOMPILE),)
	LIB_CFLAGS := -shared -fPIC -fvisibility=hidden
	SO_LDFLAGS := -Wl,-soname,libfalcon.so.0
else
	ifeq ($(KERNEL_NAME), Linux)
		LIB_CFLAGS := -shared -fPIC -fvisibility=hidden
		SO_LDFLAGS := -Wl,-soname,libfalcon.so.0
	endif
	ifeq ($(KERNEL_NAME), Darwin)
		LIB_CFLAGS := -dynamiclib -undefined dynamic_lookup
	endif
	ifeq ($(KERNEL_NAME), $(filter $(KERNEL_NAME),OpenBSD FreeBSD NetBSD))
		LIB_CFLAGS := -shared -fPIC
	endif
endif

all: $(LIB_NAME)

$(LIB_NAME): $(SRC)
	mkdir -p $(MIX_APP_PATH)/priv
	$(CC) $(CFLAGS) $(LIB_CFLAGS) $(SO_LDFLAGS) $^ -o $@

clean:
	rm -f $(LIB_NAME)

.PHONY: all clean
