CC = g++
CFLAGS = -c -g -Wshadow -Winit-self -Wredundant-decls -Wcast-align\
-Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations\
-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra\
-Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy\
-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op\
-Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith\
-Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel\
-Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

EXEDIR = ../
all: $(EXEDIR)/compiler.exe $(EXEDIR)/discompiler.exe $(EXEDIR)/executor.exe

$(EXEDIR)/compiler.exe: compiler.o
	$(CC) compiler.o -o $(EXEDIR)/compiler.exe

compiler.o: compiler.cpp
	$(CC) $(CFLAGS) compiler.cpp

$(EXEDIR)/discompiler.exe: discompiler.o
	$(CC) discompiler.o -o $(EXEDIR)/discompiler.exe

discompiler.o: discompiler.cpp
	$(CC) $(CFLAGS) discompiler.cpp

$(EXEDIR)/executor.exe: executor.o stack.o
	$(CC) executor.o stack.o -o $(EXEDIR)/executor.exe

executor.o: executor.cpp
	$(CC) $(CFLAGS) executor.cpp

stack.o: stack.cpp
	$(CC) $(CFLAGS) stack.cpp

.PHONY: clean all

clean:
	del $(EXEDIR)/compiler.exe $(EXEDIR)/discompiler.exe $(EXEDIR)/executor.exe
	del compiler.o discompiler.o executor.o
