CC := clang

INCLUDES += -I"."

DEFINES += -D"ITRLIB_DEBUG"
#DEFINES += -D"DDS_MAIN"

SOURCES += example/main.c
SOURCES += itrlib/itrlog/itrlog.c
SOURCES += itrlib/itrobj/itrobj.c
SOURCES += ddslib/ddsobj/ddsobj.c
SOURCES += ddslib/ddslist/ddslist.c

CFLAGS += -ggdb
CFLAGS += -O0
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Werror

DIR_OUT := build
NAME_OUT := example
LOGNAME_OUT := ITR_OUTPUT.log

all:
	@mkdir -p $(DIR_OUT)
	$(CC) $(DEFINES) $(INCLUDES) $(CFLAGS) $(SOURCES) -o $(DIR_OUT)/$(NAME_OUT).out

clean:
	@rm -rf $(DIR_OUT) $(LOGNAME_OUT)
