SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

INCS = -Iinclude

TARGET = libqueue.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $(TARGET) $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
