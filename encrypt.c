#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <libgen.h>
#endif

// Encrypt: shift 1 forward
char encrypt_char(char ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return ((ch - 'A' + 1) % 26) + 'A';
    }
    else if (ch >= 'a' && ch <= 'z') {
        return ((ch - 'a' + 1) % 26) + 'a';
    }
    else if (ch >= '0' && ch <= '9') {
        return ((ch - '0' + 1) % 10) + '0';
    }
    else {
        return ch;
    }
}

// Decrypt: shift 1 backward
char decrypt_char(char ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return ((ch - 'A' - 1 + 26) % 26) + 'A';
    }
    else if (ch >= 'a' && ch <= 'z') {
        return ((ch - 'a' - 1 + 26) % 26) + 'a';
    }
    else if (ch >= '0' && ch <= '9') {
        return ((ch - '0' - 1 + 10) % 10) + '0';
    }
    else {
        return ch;
    }
}

// Get absolute path (cross-platform)
char* get_absolute_path(const char *path) {
    static char abs_path[512];
    #ifdef _WIN32
        if (_fullpath(abs_path, path, sizeof(abs_path)) == NULL) {
            strncpy(abs_path, path, sizeof(abs_path) - 1);
            abs_path[sizeof(abs_path) - 1] = '\0';
        }
    #else
        if (realpath(path, abs_path) == NULL) {
            strncpy(abs_path, path, sizeof(abs_path) - 1);
            abs_path[sizeof(abs_path) - 1] = '\0';
        }
    #endif
    return abs_path;
}

void process_file(char input[], char output[], int encrypt) {
    FILE *fin = fopen(input, "r");
    if (fin == NULL) {
        fprintf(stderr, "Could not open input file '%s': %s\n", input, strerror(errno));
        return;
    }

    FILE *fout = fopen(output, "w");
    if (fout == NULL) {
        fprintf(stderr, "Could not open output file '%s': %s\n", output, strerror(errno));
        fclose(fin);
        return;
    }

    int c;
    while ((c = fgetc(fin)) != EOF) {
        char ch = (char)c;
        if (encrypt == 1)
            fputc(encrypt_char(ch), fout);
        else
            fputc(decrypt_char(ch), fout);
    }

    fclose(fin);
    fclose(fout);

    char *abs_out = get_absolute_path(output);
    if (encrypt == 1)
        printf("Encryption complete! Output saved to:\n  %s\n", abs_out);
    else
        printf("Decryption complete! Output saved to:\n  %s\n", abs_out);
}

int main(int argc, char **argv) {
    int choice = 0;
    char input_filename[256], output_filename[256];

    /* Command-line mode */
    if (argc == 4) {
        if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--encrypt") == 0) {
            choice = 1;
        } else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--decrypt") == 0) {
            choice = 2;
        } else {
            fprintf(stderr, "Invalid option '%s'.\n", argv[1]);
            fprintf(stderr, "Usage: %s [-e|-d] input_file output_file\n", argv[0]);
            return 1;
        }

        strncpy(input_filename, argv[2], sizeof(input_filename) - 1);
        input_filename[sizeof(input_filename) - 1] = '\0';
        strncpy(output_filename, argv[3], sizeof(output_filename) - 1);
        output_filename[sizeof(output_filename) - 1] = '\0';

        process_file(input_filename, output_filename, choice);
        return 0;
    }

    /* Interactive fallback */
    printf("1. Encrypt a file\n");
    printf("2. Decrypt a file\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Failed to read choice.\n");
        return 1;
    }

    if (choice != 1 && choice != 2) {
        printf("Invalid choice!\n");
        return 0;
    }

    printf("Enter input file name: ");
    scanf("%255s", input_filename);

    printf("Enter output file name: ");
    scanf("%255s", output_filename);

    process_file(input_filename, output_filename, choice);

    return 0;
}
