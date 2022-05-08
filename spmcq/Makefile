include compile.mk
include config.mk

TOP = $(shell pwd)

INCS = -Iinclude -Iqueue/include
LIBS = -lpthread queue/libqueue.a

SUBDIRS = queue
TARGET = spmcq

BUILDSUBDIRS = $(SUBDIRS:%=build-%)
CLEANSUBDIRS = $(SUBDIRS:%=clean-%)

export TOP

.PHONY: all clean $(DEPDIR) $(BUILDSUBDIRS) $(CLEANSUBDIRS) format

all: $(DEPDIR) $(BUILDSUBDIRS) $(TARGET)

$(DEPDIR):
	mkdir -p $@

$(BUILDSUBDIRS):
	$(MAKE) -C $(@:build-%=%)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) $(DEPFLAGS) $(CFLAGS) $(INCS)  -c $< -o $@

clean: $(CLEANSUBDIRS)
	rm -f $(TARGET) $(OBJS)

$(CLEANSUBDIRS):
	$(MAKE) -C $(@:clean-%=%) clean

# TODO: Use automatically when pre-commmit
format:
	astyle --style=linux --recursive -n "*.c" "*.h"

-include $(DEPFILES)
