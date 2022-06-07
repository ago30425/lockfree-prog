include compile.mk
include config.mk

TOP = $(shell pwd)

# libspmcq.so
LIBSPMCQ_PATH = ../../spmcq
LIBSPMCQ = libspmcq
CLEAN_LIBSPMCQ = clean_libspmcq

INCS = -I$(LIBSPMCQ_PATH)/include
LIBS = -lpthread $(LIBSPMCQ_PATH)/$(LIBSPMCQ).so

TARGET = queue_test

export TOP

.PHONY: all clean $(LIBSPMCQ) $(CLEAN_LIBSPMCQ) format

all: $(DEPDIR) $(LIBSPMCQ) $(TARGET)

$(DEPDIR):
	mkdir -p $@

$(LIBSPMCQ):
	$(MAKE) -C $(LIBSPMCQ_PATH)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) $(DEPFLAGS) $(CFLAGS) $(INCS)  -c $< -o $@

clean: $(CLEAN_LIBSPMCQ)
	rm -f $(TARGET) $(OBJS)

$(CLEAN_LIBSPMCQ):
	$(MAKE) -C $(LIBSPMCQ_PATH) clean

# TODO: Use automatically when pre-commmit
format:
	astyle --style=linux --recursive -n "*.c" "*.h"

-include $(DEPFILES)
