include $(TOP)/compile.mk
include $(TOP)/config.mk

INCS = -Iinclude -Iinclude/internal

TARGET = libqueue.a

.PHONY: all clean $(DEPDIR)

all: $(DEPDIR) $(TARGET)

$(DEPDIR):
	mkdir -p $@

$(TARGET): $(OBJS)
	$(AR) rcs $(TARGET) $^

%.o: %.c
	$(CC) $(DEPFLAGS) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

-include $(DEPFILES)
