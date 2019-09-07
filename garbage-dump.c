#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Note: ((rand() % (max + 1 - min)) + min) is used to generate random
//       characters within a certain range.

#define FILENAME_LENGTH 20

// Generates a random string of set size.
void generateRandomString(char* buffer, size_t size);
// Generates a random file name consisting of uppercase characters.
void generateRandomFilename(char* buffer, size_t size);

int main(int argc, char* argv[]) {
	int exitCode = 0;

	// Too few arguments error handling.
	if (argc < 4) {
		fprintf(stderr,
		        "Usage: garbage-dump <target location> <bytes to fill> "
		        "<file size>");
		exitCode = 1;
	} else {
		// Program parameters.
		char* targetLocation = argv[1];
		size_t capacity = 0, chunkSize = 1, numberOfFiles;

		sscanf(argv[2], '%zu', capacity);
		sscanf(argv[3], '%zu', chunkSize);
		numberOfFiles = capacity / chunkSize;

		if (capacity) {
			if (chunkSize) {
				// Initialize the random number generator.
				time_t currentTime;
				time(&currentTime);
				srand(currentTime);

				char fileMode[2];
				strcpy(fileMode, "w");

				FILE* file;

				char currentFile[PATH_MAX];
				char* filename = malloc(sizeof(char) * FILENAME_LENGTH);
				char* chars = malloc(sizeof(char) * chunkSize);

				// Ensure filename and chars were allocated before continuing.
				if (filename && chars) {
					for (int i = 0; i < numberOfFiles; i++) {
						// Construct the full file path.
						generateRandomFilename(filename, FILENAME_LENGTH);
						strncpy(currentFile, targetLocation,
								PATH_MAX - 1 - FILENAME_LENGTH);
						strcat(currentFile, "/");
						strncat(currentFile, filename, FILENAME_LENGTH);

						// Re-generate the filename until one
						// is generated that doesn't already exist.
						while (access(currentFile, F_OK) != -1) {
							generateRandomFilename(filename, FILENAME_LENGTH);
							strncpy(currentFile, targetLocation,
									PATH_MAX - 1 - FILENAME_LENGTH);
							strcat(currentFile, "/");
							strncat(currentFile, filename, FILENAME_LENGTH);
						}

						// Open current target file for writing.
						file = fopen(currentFile, fileMode);

						if (file) {
							generateRandomString(chars, chunkSize);
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

void generateRandomString(char* buffer, size_t size) {
	if (buffer) {
		for (int i = 0; i < (size - 1); ++i) {
			buffer[i] = (rand() % 255 + 1 - 1) + 1;
		}

		buffer[size - 1] = 0;
	}
}

void generateRandomFilename(char* buffer, size_t size) {
	if (buffer) {
		for (int i = 0; i < (size - 1); ++i) {
			buffer[i] = (rand() % (90 + 1 - 65)) + 65;
		}

		buffer[size - 1] = 0;
	}
}