CC=gcc
OBJFILES=$(wildcard ./*.c)
INTERMEDIARY=main
TARGET=demo

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -I ../include $(OBJFILES) -o $(INTERMEDIARY) -L../ -lhashed

clean:
	rm $(TARGET)
