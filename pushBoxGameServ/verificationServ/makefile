GCC ?= g++
CCMODE = PROGRAM
INCLUDES = -I/usr/include/mysql
CFLAGS = -Wall -std=c++11 $(MACRO)
TARGET = VerificationSrv
SRCS := $(wildcard *.cpp)
LIBS = -lmysqlclient

ifeq ($(CCMODE), PROGRAM)
$(TARGET): $(SRCS)
	$(GCC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LIBS)
	@chmod +x $(TARGET)
	@echo make $(TARGET) ok.

clean:
	rm -rf $(TARGET)
endif
