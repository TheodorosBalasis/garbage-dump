#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// EXIT CODES
// 0: Program executed correctly.
// 1: Invalid number of arguments.
// 2: Target directory not found.
// 3: Directory could not be accessed.
// 4: Provided chunk size is 0.
// 5: Failure to allocate buffers for filename and file data.

// Note: ((rand() % (max + 1 - min)) + min) is used to generate random
//       characters within a certain range.

const size_t FILENAME_LENGTH = 20;
const char FILENAME_LEAST_CHAR = 65;
const char FILENAME_GREATEST_CHAR = 90;

void generate_random_string(char *buffer, size_t buffer_size, char lower_bound,
                            char upper_bound);
int directory_exists(char *path);

int main(int argc, char *argv[]) {
  int exit_code = 0;

  // Error handling.
  if (argc != 4) {
    fprintf(stderr, "Usage: garbage-dump <target location> <bytes to fill> "
                    "<file size>\n");
    exit_code = 1;
  } else if (!directory_exists(argv[1])) {
    fprintf(stderr, "Directory not found.\n");
    exit_code = 2;
  } else if (directory_exists(argv[1]) == -1) {
    fprintf(stderr, "Directory could not be accessed.\n");
    exit_code = 3;
  } else {
    // Program parameters.
    char *target_location = argv[1];
    size_t capacity = 0, chunk_size = 1, number_of_files;

    sscanf(argv[2], "%zu", &capacity);
    sscanf(argv[3], "%zu", &chunk_size);
    number_of_files = capacity / chunk_size;

    if (capacity) {
      if (!chunk_size) {
        fprintf(stderr, "Invalid chunk size!\n");
        free(target_location);
        exit_code = 4;
      } else {
        // Initialize the random number generator.
        time_t current_time;
        time(&current_time);
        srand(current_time);

        char* file_mode = "w";

        FILE *file;

        char current_file[PATH_MAX];
        char *filename = malloc(sizeof(char) * FILENAME_LENGTH);
        char *chars = malloc(sizeof(char) * chunk_size);

        // Ensure filename and chars were allocated before continuing.
        if (!(filename && chars)) {
          fprintf(stderr, "Could not allocate buffers for filename and file "
                          "data.\n");
          exit_code = 5;
        } else {
          for (int i = 0; i < number_of_files; i++) {
            // Construct the full file path.
            generate_random_string(filename, FILENAME_LENGTH,
                                   FILENAME_LEAST_CHAR, FILENAME_GREATEST_CHAR);
            strncpy(current_file, target_location,
                    PATH_MAX - 1 - FILENAME_LENGTH);
            strcat(current_file, "/");
            strncat(current_file, filename, FILENAME_LENGTH);

            // Re-generate the filename until one
            // is generated that doesn't already exist.
            while (access(current_file, F_OK) != -1) {
              generate_random_string(filename, FILENAME_LENGTH,
                                     FILENAME_LEAST_CHAR,
                                     FILENAME_GREATEST_CHAR);
              strncpy(current_file, target_location,
                      PATH_MAX - 1 - FILENAME_LENGTH);
              strcat(current_file, "/");
              strncat(current_file, filename, FILENAME_LENGTH);
            }

            // Open current target file for writing.
            file = fopen(current_file, file_mode);

            if (file) {
              generate_random_string(chars, chunk_size, (char)1, (char)255);
              fputs(chars, file);
              fclose(file);
            } else {
              fprintf(stderr, "Could not open file %s for writing\n",
                      current_file);
            }
          }

          free(filename);
          free(chars);
        }
      }
    }
  }

  return exit_code;
}

void generate_random_string(char *buffer, size_t buffer_size, char lower_bound,
                            char upper_bound) {
  if (buffer) {
    for (int i = 0; i < (buffer_size - 1); ++i) {
      buffer[i] = (rand() % (upper_bound + 1 - lower_bound)) + lower_bound;
    }

    buffer[buffer_size - 1] = 0;
  }
}

int directory_exists(char *path) {
  int exists = 1;
  DIR *directory = opendir(path);

  if (directory) {
    closedir(directory);
  } else if (ENOENT == errno) {
    exists = 0;
  } else {
    exists = -1;
  }

  return exists;
}