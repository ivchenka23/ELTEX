CC = gcc
CFLAGS = -Wall -std=c11 -fPIC
LDFLAGS = -lm -ldl
PLUGIN_DIR = plugins
TARGET = calculator

MAIN_SRC = menu.c plugin_manager.c

all: $(TARGET) dir plugins

$(TARGET): $(MAIN_SRC) calculator.h
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN_SRC) $(LDFLAGS)

dir:
	mkdir -p $(PLUGIN_DIR)

plugins: dir
	$(CC) $(CFLAGS) -shared -o $(PLUGIN_DIR)/plugin_add.so plugin_add.c
	$(CC) $(CFLAGS) -shared -o $(PLUGIN_DIR)/plugin_sub.so plugin_sub.c
	$(CC) $(CFLAGS) -shared -o $(PLUGIN_DIR)/plugin_mul.so plugin_mul.c
	$(CC) $(CFLAGS) -shared -o $(PLUGIN_DIR)/plugin_div.so plugin_div.c
	$(CC) $(CFLAGS) -shared -o $(PLUGIN_DIR)/plugin_pow.so plugin_pow.c -lm
	$(CC) $(CFLAGS) -shared -o $(PLUGIN_DIR)/plugin_sqrt.so plugin_sqrt.c -lm

clean:
	rm -f $(TARGET)
	rm -rf $(PLUGIN_DIR)
	
.PHONY: all clean dir plugins
