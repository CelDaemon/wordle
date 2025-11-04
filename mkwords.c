#include <getopt.h>
#include <stddef.h>

int main(int argc, char *argv[]) {
    return getopt_long(argc, argv, NULL, NULL, NULL);
}
