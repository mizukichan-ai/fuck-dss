CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2
TARGET = fuck-dss
SOURCE = fuck-dss.c

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

test: $(TARGET)
	@echo "Testing fuck-dss..."
	@mkdir -p test_dir/.DS_Store test_dir/.Spotlight-V100 test_dir/.Trashes test_dir/subdir/._test
	@touch test_dir/test.txt test_dir/.DS_Store test_dir/.Spotlight-V100 test_dir/.Trashes test_dir/subdir/._test
	@echo "Created test files:"
	@ls -la test_dir/
	@echo ""
	@echo "Running fuck-dss (should remove DSS files):"
	@./$(TARGET) -v test_dir
	@echo ""
	@echo "Files remaining:"
	@ls -la test_dir/
	@echo ""
	@echo "Cleaning up test directory..."
	@rm -rf test_dir

distclean: clean
	rm -rf test_dir