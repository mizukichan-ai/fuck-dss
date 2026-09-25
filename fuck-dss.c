#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_PATH 4096

typedef struct {
    bool verbose;
    bool no_crawl;
    bool ignore_dss;
    bool ignore_spotlight;
    bool ignore_trash;
} Config;

typedef struct {
    char path[MAX_PATH];
    const char *name;
} FileEntry;

// macOS-specific file patterns
const char *dss_patterns[] = {".DS_Store", "._*", ".AppleDouble", ".DS_Store?", NULL};
const char *spotlight_patterns[] = {".Spotlight-V100", ".fseventsd", ".metadata_never_index", NULL};
const char *trash_patterns[] = {".Trashes", ".Trash", NULL};

bool matches_pattern(const char *filename, const char *pattern) {
    if (pattern[0] == '.') {
        if (pattern[1] == '*') {
            // Wildcard pattern like "._*"
            return strncmp(filename, pattern, 1) == 0;
        } else if (pattern[1] == '?') {
            // Pattern like ".DS_Store?"
            return strncmp(filename, pattern, 2) == 0 && strlen(filename) >= 2;
        }
    }
    return strcmp(filename, pattern) == 0;
}

bool should_remove(const char *filename, Config *config) {
    if (!config->ignore_dss) {
        for (int i = 0; dss_patterns[i] != NULL; i++) {
            if (matches_pattern(filename, dss_patterns[i])) {
                return true;
            }
        }
    }
    
    if (!config->ignore_spotlight) {
        for (int i = 0; spotlight_patterns[i] != NULL; i++) {
            if (strcmp(filename, spotlight_patterns[i]) == 0) {
                return true;
            }
        }
    }
    
    if (!config->ignore_trash) {
        for (int i = 0; trash_patterns[i] != NULL; i++) {
            if (strcmp(filename, trash_patterns[i]) == 0) {
                return true;
            }
        }
    }
    
    return false;
}

void remove_file(const char *path, Config *config) {
    if (remove(path) == 0) {
        if (config->verbose) {
            printf("Removed: %s\n", path);
        }
    } else {
        fprintf(stderr, "Error removing %s: %s\n", path, strerror(errno));
    }
}

void process_directory(const char *dir_path, Config *config) {
    DIR *dir;
    struct dirent *entry;
    char full_path[MAX_PATH];
    
    dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Error opening directory %s: %s\n", dir_path, strerror(errno));
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        snprintf(full_path, MAX_PATH, "%s/%s", dir_path, entry->d_name);
        
        struct stat statbuf;
        if (stat(full_path, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                if (!config->no_crawl) {
                    process_directory(full_path, config);
                }
            } else if (S_ISREG(statbuf.st_mode)) {
                if (should_remove(entry->d_name, config)) {
                    remove_file(full_path, config);
                }
            }
        }
    }
    
    closedir(dir);
}

void print_usage(const char *program_name) {
    printf("Usage: %s -<flags> <targetdir>\n", program_name);
    printf("Flags:\n");
    printf("  v: Report program activity to stdout\n");
    printf("  n: Don't crawl\n");
    printf("  d: Ignore DSS files\n");
    printf("  s: Ignore Spotlight files\n");
    printf("  t: Ignore trash files\n");
}

int main(int argc, char *argv[]) {
    Config config = {false, false, false, false, false};
    char target_dir[MAX_PATH];
    
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Parse flags
    for (int i = 1; i < argc - 1; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'v':
                        config.verbose = true;
                        break;
                    case 'n':
                        config.no_crawl = true;
                        break;
                    case 'd':
                        config.ignore_dss = true;
                        break;
                    case 's':
                        config.ignore_spotlight = true;
                        break;
                    case 't':
                        config.ignore_trash = true;
                        break;
                    default:
                        fprintf(stderr, "Unknown flag: -%c\n", argv[i][j]);
                        print_usage(argv[0]);
                        return 1;
                }
            }
        }
    }
    
    // Get target directory
    strncpy(target_dir, argv[argc - 1], MAX_PATH - 1);
    target_dir[MAX_PATH - 1] = '\0';
    
    if (config.verbose) {
        printf("Starting fuck-dss in directory: %s\n", target_dir);
        printf("Flags: v=%d, n=%d, d=%d, s=%d, t=%d\n", 
               config.verbose, config.no_crawl, config.ignore_dss, 
               config.ignore_spotlight, config.ignore_trash);
    }
    
    process_directory(target_dir, &config);
    
    if (config.verbose) {
        printf("fuck-dss completed.\n");
    }
    
    return 0;
}
