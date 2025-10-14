#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include <zlib.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <termios.h>
#endif

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

void set_echo(bool echo) {
#ifdef _WIN32
    HANDLE std = GetStdHandle(STD_INPUT_HANDLE);
    DWORD flags;
    GetConsoleMode(std, &flags);
    if(echo)
        flags |= ENABLE_ECHO_INPUT;
    else
        flags &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode(std, flags);
#else
    struct termios ts;
    tcgetattr(STDIN_FILENO, &ts);
    if(echo)
        ts.c_lflag |= ECHO;
    else
        ts.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ts);
#endif
}

void set_canon(bool canon) {
#ifdef _WIN32
    HANDLE std = GetStdHandle(STD_INPUT_HANDLE);
    DWORD flags;
    GetConsoleMode(std, &flags);
    if(canon)
        flags |= ENABLE_LINE_INPUT;
    else
        flags &= ~ENABLE_LINE_INPUT;
    SetConsoleMode(std, flags);
#else
    struct termios ts;
    tcgetattr(STDIN_FILENO, &ts);
    if(canon)
        ts.c_lflag |= ICANON;
    else
        ts.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &ts);

#endif
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
        fprintf(stderr, "Failed to find words.db.gz\n");
        return false;
    }

    gzFile const file = gzopen(path, "rb");
    
    if(file == NULL) {
        perror("Failed to open file");
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
        if(bytes_read == -1) {
            fputs(gzerror(file, NULL), stderr);
            goto free_list;
        }
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


        fputs("Enter your guess: ", stdout);

        char guess[WORD_SIZE];
        char guess_index = 0;

        set_echo(false);
        set_canon(false);

        while(true) {

            //fputs("Enter your guess: ", stdout);

            //if(fgets(guess, sizeof(guess), stdin) == NULL)
            //    return 1;

            //cursor_up();
            //erase_line(EL_WHOLE);

            //int guess_length = strnlen(guess, sizeof(guess)) - 1;

            //if(guess_length != WORD_SIZE || !isalphas(guess, WORD_SIZE))
            //    continue;

            //for(int y = 0; y < WORD_SIZE; y++)
            //    guess[y] = toupper(guess[y]);

            //if(!guess_exists(guess, word_list, word_count))
            //    continue;

            //break;
            //
            
            int guess_char = getchar();

            if(guess_index < WORD_SIZE && isalpha(guess_char)) {
                putchar(guess_char);
                guess[guess_index++] = guess_char;
                continue;
            }
        }

        set_echo(true);
        set_canon(true);

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
