CPP=g++
CPP_FLAGS=-std=c++23 -Mmodules -Wall -c
LINK_FLAGS=-ffunction-sections -fdata-sections -Wl,--gc-sections -fno-exceptions -fno-rtti
LIBS=-lgmp -lgmpxx -lmpfr
#
BIN=./bin/main.o
TARGET=./bin/cyv
ARGS=--debug -g3 -DDBG

.SILENT:
./bin/%.o: ../src/%.c++ Makefile
	@echo " |--> $<"
	$(CPP) $(CPP_FLAGS) $(ARGS) $< -o $@

all: $(BIN) Makefile
	@echo " |-> linking..."
	$(CPP) $(LINK_FLAGS) $(BIN) $(LIBS) $(ARGS) -o $(TARGET)
	@echo "->Done"

opt:
	@echo " |-> minimazing..."
	strip $(TARGET)
	@echo " |-> deleting symbols..."
	upx --best --8-bit --ultra-brute $(TARGET)
	@echo "->Done"

clean:
	rm -rf ./bin/* ./out/*



dbg: ARGS=../db/test/parse.cyv
dbg:
	gdb $(TARGET) $(ARGS)



run: ARGS=../db/test/parse.cyv
run:
	$(TARGET) $(ARGS)



sz:
	ls -lh ./out



strs: NAME=""
strs:
	strings $(TARGET) | grep $(NAME)

.PHONY: all clean sz strs
