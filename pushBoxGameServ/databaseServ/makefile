GCC ?= g++
CCMODE = PROGRAM
INCLUDES = -I/opt/libevent/include/ -I/usr/include/mysql
CFLAGS = -Wall $(MACRO) -std=c++11 -DTEST
TARGET = DatabaseSrv
SRCS := $(wildcard *.cpp)
LIBS = -L /opt/libevent/lib/  -lmysqlclient -levent -lpthread

ifeq ($(CCMODE),PROGRAM)
$(TARGET): $(LINKS) $(SRCS)
	$(GCC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LIBS)
	@chmod +x $(TARGET)
	@echo make $(TARGET) ok.
endif

clean:
	rm -rf $(TARGET)

.PHONY: install
.PHONY: clean
