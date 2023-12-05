.PHONY: all clean

EXAMPLES := $(addprefix examples_build/,$(notdir $(basename $(wildcard examples/*.c))))
FILES_ALLOWED_FOR_CHANGE := $(shell cat files_allowed_for_change)
CHANGED_FILES := $(wildcard $(FILES_ALLOWED_FOR_CHANGE))
TEMPLATE_HASH := $(shell cat template_hash)
CFLAGS := --std=gnu11 -Wall -DDEBUG -lpthread
TESTS := $(wildcard tests/*.self)

CHANNEL_SRC := channel.c channel.h
MIMPI_COMMON_SRC := $(CHANNEL_SRC) mimpi_common.c mimpi_common.h
MIMPIRUN_SRC := $(MIMPI_COMMON_SRC) mimpirun.c
MIMPI_SRC := $(MIMPI_COMMON_SRC) mimpi.c mimpi.h

all: mimpirun $(EXAMPLES) $(TESTS)

mimpirun: $(MIMPIRUN_SRC)
	gcc $(CFLAGS) -o $@ $(filter %.c,$^)

examples_build/%: examples/%.c $(MIMPI_SRC)
	mkdir -p examples_build
	gcc $(CFLAGS) -o $@ $(filter %.c,$^)

assignment.zip: $(CHANGED_FILES)
	zip assignment.zip $(CHANGED_FILES)

clean:
	rm -rf mimpirun assignment.zip examples_build
