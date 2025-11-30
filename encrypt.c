
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
    #include <direct.h>
    #define GETCWD _getcwd
#else
    #include <unistd.h>
    #define GETCWD getcwd
#endif

// Encrypt: shift 1 forward
char encrypt_char(char ch) {
    if (ch >= 'A' && ch <= 'Z')
        return ((ch - 'A' + 1) % 26) + 'A';
    else if (ch >= 'a' && ch <= 'z')
        return ((ch - 'a' + 1) % 26) + 'a';
    else if (ch >= '0' && ch <= '9')
        return ((ch - '0' + 1) % 10) + '0';
    else
        return ch;
}

// Decrypt: shift 1 backward
char decrypt_char(char ch) {
    if (ch >= 'A' && ch <= 'Z')
        return ((ch - 'A' - 1 + 26) % 26) + 'A';
    else if (ch >= 'a' && ch <= 'z')
        return ((ch - 'a' - 1 + 26) % 26) + 'a';
    else if (ch >= '0' && ch <= '9')
        return ((ch - '0' - 1 + 10) % 10) + '0';
    else
        return ch;
}

void process_file(char input[], char output[], int encrypt) {
    char cwd[256];
    char full_output[512];
    
    GETCWD(cwd, sizeof(cwd));
    printf("[DEBUG] Current working directory: %s\n", cwd);
    printf("[DEBUG] Input file: %s\n", input);
    printf("[DEBUG] Output file (relative): %s\n\n", output);

    /* If output is relative (no path separators), prepend project folder */
    if (strchr(output, '\\') == NULL && strchr(output, '/') == NULL) {
        snprintf(full_output, sizeof(full_output), "C:\\Users\\sruja\\OneDrive\\Documents\\CODING\\project\\%s", output);
        printf("[DEBUG] Output redirected to: %s\n\n", full_output);
    } else {
        strncpy(full_output, output, sizeof(full_output) - 1);
        full_output[sizeof(full_output) - 1] = '\0';
    }

    FILE *fin = fopen(input, "r");
    FILE *fout = fopen(full_output, "w");

    if (fin == NULL || fout == NULL) {
        printf("Error opening files!\n");
        if (fin == NULL) printf("  Input file not found: %s\n", input);
        if (fout == NULL) printf("  Cannot create output file: %s\n", full_output);
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

    if (encrypt == 1)
        printf("Encryption done! Saved to %s\n", full_output);
    else
        printf("Decryption done! Saved to %s\n", full_output);
}

int main(int argc, char **argv) {
    int choice;
    char input_filename[100], output_filename[100];

    /* Command-line mode: encrypt input output */
    if (argc == 4 && (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "-d") == 0)) {
        choice = (strcmp(argv[1], "-e") == 0) ? 1 : 2;
        strncpy(input_filename, argv[2], sizeof(input_filename) - 1);
        strncpy(output_filename, argv[3], sizeof(output_filename) - 1);
        input_filename[sizeof(input_filename) - 1] = '\0';
        output_filename[sizeof(output_filename) - 1] = '\0';
        
        process_file(input_filename, output_filename, choice);
        return 0;
    }

    /* Interactive mode */
    printf("1. Encrypt a file\n");
    printf("2. Decrypt a file\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice != 1 && choice != 2) {
        printf("Invalid choice!\n");
        return 0;
    }

    printf("Enter input file name: ");
    scanf("%99s", input_filename);

    printf("Enter output file name: ");
    scanf("%99s", output_filename);

    process_file(input_filename, output_filename, choice);

    return 0;
}
