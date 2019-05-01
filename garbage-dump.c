#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Note: ((rand() % (max + 1 - min)) + min) is used to generate random
//       characters within a certain range.

// Generates a random string of set size.
char* randomString(size_t size);
// Generates a random file name consisting of uppercase characters.
char* randomFilename(size_t size);

int main(int argc, char* argv[]) {
	if (argc < 4) {
		fprintf(stderr,
		        "Usage: garbage-dump <target location> <bytes to fill> "
		        "<file size>");
	}

	// Program parameters.
	char* targetLocation = argv[1];
	size_t capacity = atoll(argv[2]);
	size_t chunkSize = atoll(argv[3]);
	size_t numberOfFiles = capacity / chunkSize;

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
			char* filename;

			for (int i = 0; i < numberOfFiles; i++) {
				// Construct the full file path.
				filename = randomFilename(20);
				strcpy(currentFile, targetLocation);
				strcat(currentFile, "/");
				strcat(currentFile, filename);

				// Write a random string to the current target path.
				file = fopen(currentFile, fileMode);

				if (file) {
					char* chars = randomString(chunkSize);
					fputs(chars, file);
					fclose(file);
					free(chars);
				}

				free(filename);
			}

			return 0;
		} else {
			fprintf(stderr, "Invalid chunk size!");
			free(targetLocation);
			return 1;
		}
	}

	return 0;
}

char* randomString(size_t size) {
	char* chars = malloc(sizeof(char) * size);

	if (chars) {
		for (int i = 0; i < (size - 1); i++) {
			chars[i] = (rand() % 255 + 1 - 1) + 1;
		}

		chars[size - 1] = 0;
	}

	return chars;
}

char* randomFilename(size_t size) {
	char* chars = malloc(sizeof(char) * size);

	if (chars) {
		for (int i = 0; i < (size - 1); i++) {
			chars[i] = (rand() % (90 + 1 - 65)) + 65;
		}

		chars[size - 1] = 0;
	}

	return chars;
}