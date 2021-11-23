# Cross compile
CC = gcc
AR = ar

CFLAGS = -Werror -O2
INCS = -Iqueue/include
LIBS = -lpthread queue/libqueue.a

ifeq ($(TEST), y)
	CFLAGS += -DTEST
endif

ifeq ($(DEBUG), )
	CFLAGS += -DNDEBUG
endif

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))
TARGET = spmcq

SUBDIRS = queue
BUILDSUBDIRS = $(SUBDIRS:%=build-%)
CLEANSUBDIRS = $(SUBDIRS:%=clean-%)

export CC AR CFLAGS

.PHONY: all clean $(BUILDSUBDIRS) $(CLEANSUBDIRS)

all: $(BUILDSUBDIRS) $(TARGET)

$(BUILDSUBDIRS):
	$(MAKE) -C $(@:build-%=%)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCS)  -c $< -o $@

clean: $(CLEANSUBDIRS)
	rm -f $(TARGET) $(OBJS)

$(CLEANSUBDIRS):
	$(MAKE) -C $(@:clean-%=%) clean


