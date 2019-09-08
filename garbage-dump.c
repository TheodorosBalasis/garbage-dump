#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

// EXIT CODES
// 0: Program executed correctly.
// 1: Invalid number of arguments.
// 2: Target directory not found.
// 3: Directory could not be accessed.

// Note: ((rand() % (max + 1 - min)) + min) is used to generate random
//       characters within a certain range.

const size_t FILENAME_LENGTH = 20;
const char FILENAME_LEAST_CHAR = 65;
const char FILENAME_GREATEST_CHAR = 90;

void generateRandomString(char* buffer, size_t buffer_size, char lower_bound, char upper_bound);
int directoryExists(char* path);

int main(int argc, char* argv[]) {
	int exitCode = 0;

	// Error handling.
	if (argc != 4) {
		fprintf(stderr,
		        "Usage: garbage-dump <target location> <bytes to fill> "
		        "<file size>\n");
		exitCode = 1;
	} else if (!directoryExists(argv[1])){
		fprintf(stderr, "Directory not found.\n");
		exitCode = 2;
	} else if (directoryExists(argv[1]) == -1) {
		fprintf(stderr, "Directory could not be accessed.\n");
		exitCode = 3;
	} else {
		// Program parameters.
		char* targetLocation = argv[1];
		size_t capacity = 0, chunkSize = 1, numberOfFiles;

		sscanf(argv[2], "%zu", &capacity);
		sscanf(argv[3], "%zu", &chunkSize);
		numberOfFiles = capacity / chunkSize;

		if (capacity) {
			if (chunkSize) {
				// Initialize the random number generator.
				time_t currentTime;
				time(&currentTime);
				srand(currentTime);

				char fileMode[] = "w";

				FILE* file;

				char currentFile[PATH_MAX];
				char* filename = malloc(sizeof(char) * FILENAME_LENGTH);
				char* chars = malloc(sizeof(char) * chunkSize);

				// Ensure filename and chars were allocated before continuing.
				if (filename && chars) {
					for (int i = 0; i < numberOfFiles; i++) {
						// Construct the full file path.
						generateRandomString(filename, FILENAME_LENGTH, FILENAME_LEAST_CHAR, FILENAME_GREATEST_CHAR);
						strncpy(currentFile, targetLocation,
								PATH_MAX - 1 - FILENAME_LENGTH);
						strcat(currentFile, "/");
						strncat(currentFile, filename, FILENAME_LENGTH);

						// Re-generate the filename until one
						// is generated that doesn't already exist.
						while (access(currentFile, F_OK) != -1) {
							generateRandomString(filename, FILENAME_LENGTH, FILENAME_LEAST_CHAR, FILENAME_GREATEST_CHAR);
							strncpy(currentFile, targetLocation,
									PATH_MAX - 1 - FILENAME_LENGTH);
							strcat(currentFile, "/");
							strncat(currentFile, filename, FILENAME_LENGTH);
						}

						// Open current target file for writing.
						file = fopen(currentFile, fileMode);

						if (file) {
							generateRandomString(chars, chunkSize, 1, 255);
							fputs(chars, file);
							fclose(file);
						} else {
							fprintf(stderr,
							        "Could not open file %s for writing",
							        currentFile);
						}
					}

					free(filename);
					free(chars);
				} else {
					fprintf(stderr,
					        "Could not allocate buffers for filename and file "
					        "data.");
					exitCode = 1;
				}
			} else {
				fprintf(stderr, "Invalid chunk size!");
				free(targetLocation);
				exitCode = 1;
			}
		}
	}

	return exitCode;
}

void generateRandomString(char* buffer, size_t buffer_size, char lower_bound, char upper_bound) {
	if (buffer) {
		for (int i = 0; i < (buffer_size - 1); ++i) {
			buffer[i] = (rand() % (upper_bound + 1 - lower_bound)) + lower_bound;
		}

		buffer[buffer_size - 1] = 0;
	}
}

int directoryExists(char* path) {
	int exists = 1;
	DIR* directory = opendir(path);
	
	if (directory) {
		closedir(directory);
	} else if (ENOENT == errno) {
		exists = 0;
	} else {
		exists = -1;
	}

	return exists;
}