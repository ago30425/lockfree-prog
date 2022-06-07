ifeq ($(TEST), y)
	CFLAGS += -DTEST
endif

ifeq ($(DEBUG), )
	CFLAGS += -DNDEBUG
endif

