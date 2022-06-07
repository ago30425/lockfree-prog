include compile.mk
include config.mk

INCS = -Iinclude -Iinclude/internal

TARGET = libspmcq.so

.PHONY: all clean

all: $(DEPDIR) $(TARGET)

$(DEPDIR):
	mkdir -p $@

$(TARGET): $(OBJS)
	$(CC) -shared $^ -o $@

%.o: %.c
	$(CC) $(DEPFLAGS) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

-include $(DEPFILES)
