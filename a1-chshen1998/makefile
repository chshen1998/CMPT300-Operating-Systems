TARGET: my_current_processes my_memory_util my_version

all: $(TARGET)

$(TARGET): $(TARGET).c
	gcc -g -Wall $(TARGET).c -o $(TARGET)

clean:
	$(RM) my_current_processes my_memory_util my_version *.o *.exe