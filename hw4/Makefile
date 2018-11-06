CC := gcc
SRCD := src
TSTD := tests
BLDD := build
BIND := bin
INCD := include
UTILD := util
SPOOLD := spool
LIBD := lib

ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_LIBF := $(shell find $(LIBD) -type f -name *.o)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))
FUNC_FILES := $(filter-out build/main.o, $(ALL_OBJF))

TEST_SRC := $(shell find $(TSTD) -type f -name *.c)

INC := -I $(INCD)

CFLAGS := -Wall -Werror -Wno-unused-function -MMD
COLORF := -DCOLOR
DFLAGS := -g -DDEBUG -DCOLOR
PRINT_STAMENTS := -DERROR -DSUCCESS -DWARN -DINFO

STD := -std=c99
POSIX := -D_POSIX_SOURCE
BSD := -D_BSD_SOURCE
TEST_LIB := -lcriterion
LIBS := -lreadline -lm

CFLAGS += $(STD) $(POSIX) $(BSD)

EXEC := imprimer
TEST := $(EXEC)_tests

.PHONY: clean all setup debug

all: setup $(BIND)/$(EXEC) $(BIND)/$(TEST)

debug: CFLAGS += $(DFLAGS) $(PRINT_STAMENTS) $(COLORF)
debug: all

setup: $(BIND) $(BLDD) $(SPOOLD)
$(BIND):
	mkdir -p $(BIND)
$(BLDD):
	mkdir -p $(BLDD)
$(SPOOLD):
	mkdir -p $(SPOOLD)

$(BIND)/$(EXEC): $(ALL_OBJF) $(ALL_LIBF)
	$(CC) $^ -o $@ $(LIBS)

$(BIND)/$(TEST): $(FUNC_FILES) $(TEST_SRC) $(ALL_LIBF)
	$(CC) $(CFLAGS) $(INC) $(FUNC_FILES) $(TEST_SRC) $(ALL_LIBF) $(TEST_LIB) $(LIBS) -o $@

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	rm -rf $(BLDD) $(BIND)

clean_spool:
	rm -rf $(SPOOLD)

stop_printers: $(UTILD)/stop_printers.sh
	$(BASH) $(UTILD)/stop_printers.sh

show_printers: $(UTILD)/show_printers.sh
	$(BASH) $(UTILD)/show_printers.sh

.PRECIOUS: $(BLDD)/*.d
-include $(BLDD)/*.d
