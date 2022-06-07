include compile.mk
include config.mk

INCS = -Iinclude -Iinclude/internal

TARGET = libqueue.so

.PHONY: all clean $(DEPDIR)

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
