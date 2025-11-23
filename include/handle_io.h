#ifndef HANDLE_IO_H
#define HANDLE_IO_H

typedef enum {
    IO_OK,
    IO_EXIT
} IOStatus;

IOStatus handle_io(const char *input);
int ends_with_colon(const char *s);
int is_empty_line(const char *s);


#endif
