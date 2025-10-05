#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include <zlib.h>

#define WORD_SIZE 5
#define MAX_ATTEMPTS 6

#define ALPHABET_SIZE ('Z' - 'A')
#define CHAR_ALPHABET_INDEX(x) ((x) - 'A')
#define ALPHABET_INDEX_CHAR(x) ((x) + 'A')

#define CSI "\e["

#define CUU "A"
#define EL_WHOLE 2
#define EL "K"

#define SGR "m"

#define SGR_RESET 0

#define GRAY 90
#define GREEN 92
#define YELLOW 93

#define DEBUG_DB_PATH "words.db.gz"
#define DB_PATH PKGDATADIR "/words.db.gz"


void print_style(int style) {
    printf(CSI "%d" SGR, style);
}

void erase_line(int arg) {
    printf(CSI "%d" EL, arg);
}

void cursor_up() {
    fputs(CSI CUU, stdout);
}

bool load_words(char **out_word_list, int *word_count) {
#define CHUNK_SIZE 40960
    assert(out_word_list != NULL);
    assert(word_count != NULL);
    char const * path = NULL;
    if(access(DEBUG_DB_PATH, F_OK) == 0)
        path = DEBUG_DB_PATH;
    else if(access(DB_PATH, F_OK) == 0)
        path = DB_PATH;
    else {
        fprintf(stderr, "Failed to find words.db.gz");
        return false;
    }

    gzFile const file = gzopen(path, "rb");
    
    if(file == NULL) {
        fprintf(stderr, "Failed to open file\n");
        return false;
    }
    uint8_t buffer[CHUNK_SIZE];

    int capacity = CHUNK_SIZE;
    char * word_list = malloc(capacity);
    if(word_list == NULL) {
        fprintf(stderr, "Failed to allocate buffer\n");
        goto close_file;
    }
    int cursor = 0;
    int bytes_read;
    do {
        bytes_read = gzread(file, &buffer, CHUNK_SIZE);
        for(int i = 0; i < bytes_read; i++) {
            if(!isalpha(buffer[i])) {
                fprintf(stderr, "Invalid character: (%d) index %d\n", buffer[i], cursor + i);
                goto free_list;
            }
        }
        if(cursor + bytes_read > capacity) {
            capacity += bytes_read;
            char * const new_word_list = realloc(word_list, capacity);
            if(new_word_list == NULL) {
                fprintf(stderr, "Failed to increase capacity\n");
                goto free_list;
            }
            word_list = new_word_list;
        }
        memcpy(&word_list[cursor], &buffer, bytes_read);
        cursor += bytes_read;
    } while(bytes_read == CHUNK_SIZE);
    int error_code;
    auto const error_message = gzerror(file, &error_code);
    if(error_code != Z_OK) {
        fputs(error_message, stderr);
        goto free_list;
    }
    *out_word_list = word_list;
    *word_count = cursor / WORD_SIZE;
    gzclose(file);
    return true;
free_list:
    free(word_list);
close_file:
    gzclose(file);
    return false;
#undef CHUNK_SIZE
}


void print_guess(char const guess[WORD_SIZE], int const colors[WORD_SIZE]) {
    assert(guess != NULL);
    assert(colors != NULL);
    int current_color = 0;
    for(int i = 0; i < WORD_SIZE; i++) {
        int letter_color = colors[i];
        print_style(letter_color);
        putchar(guess[i]);
    }
    print_style(SGR_RESET);
    putchar('\n');
}

void process_colors(char const guess[WORD_SIZE], char const answer[WORD_SIZE], int const answer_counts[ALPHABET_SIZE], int colors[WORD_SIZE]) {
    assert(guess != NULL);
    assert(answer != NULL);
    assert(answer_counts != NULL);
    assert(colors != NULL);

    int counts[ALPHABET_SIZE];
    memcpy(counts, answer_counts, sizeof(counts));

    for(int i = 0; i < WORD_SIZE; i++) {
        if(guess[i] != answer[i])
            continue;
        colors[i] = GREEN;
        counts[CHAR_ALPHABET_INDEX(guess[i])]--;
    }
    for(int i = 0; i < WORD_SIZE; i++) {
        if(colors[i] == GREEN || counts[CHAR_ALPHABET_INDEX(guess[i])] <= 0)
            continue;
        colors[i] = YELLOW;
        counts[CHAR_ALPHABET_INDEX(guess[i])]--;
    }
}

char *isalphas(char * const text, int const text_length) {
    for(int i = 0; i < text_length; i++) {
        if(!isalpha(text[i]))
            return NULL;
    }
    return text;
}

bool guess_exists(char const guess[WORD_SIZE], char const *word_list, int const word_count) {
    for(int i = 0; i < word_count; i++) {
        if(strncmp(guess, &word_list[i * WORD_SIZE], WORD_SIZE) == 0)
            return true;
    }
    return false;
}

int main() {

    srand(time(NULL));

    char *word_list;
    int word_count;

    if(!load_words(&word_list, &word_count))
        return 0;

    char const * const answer = &word_list[rand() % word_count * WORD_SIZE];

    int answer_counts[ALPHABET_SIZE] = { 0 };

    for(int i = 0; i < WORD_SIZE; i++)
        answer_counts[CHAR_ALPHABET_INDEX(answer[i])]++;

    for(int i = 0; i < MAX_ATTEMPTS; i++) {

        char guess[WORD_SIZE + 10];

        while(true) {

            fputs("Enter your guess: ", stdout);

            if(fgets(guess, sizeof(guess), stdin) == NULL)
                return 1;

            cursor_up();
            erase_line(EL_WHOLE);

            int guess_length = strnlen(guess, sizeof(guess)) - 1;

            if(guess_length != WORD_SIZE || !isalphas(guess, WORD_SIZE))
                continue;

            for(int y = 0; y < WORD_SIZE; y++)
                guess[y] = toupper(guess[y]);

            if(!guess_exists(guess, word_list, word_count))
                continue;

            break;
        }

        int colors[WORD_SIZE] = { GRAY, GRAY, GRAY, GRAY, GRAY };

        process_colors(guess, answer, answer_counts, colors);
        print_guess(guess, colors);

        if(strncmp(guess, answer, WORD_SIZE) == 0) {
            puts("Congratulations, you won!");
            return 0;
        }
    }
    puts("You big dummy, you lost >:3");
    printf("The word was: %.5s\n", answer);
    return 1;
}
