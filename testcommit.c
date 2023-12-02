#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
typedef struct {
  char* base;
  unsigned int size;
  unsigned int used;
} ByteBuffer;

int flag_newproduce_was_called = 0;

ByteBuffer allocBuffer(unsigned int size) {
  ByteBuffer buf;

  buf.base = malloc(size);          // character buffer
  buf.size = size;
  buf.used = 0;
  return buf;
} // From Recitation

void reallocBuffer(ByteBuffer* buf, unsigned int newSize) {
  buf->base = realloc(buf->base, newSize);
  buf->size = newSize;
}   // From Recitation

int readIntoBuffer(ByteBuffer* buf, int fd) {
  int result = read(fd, buf->base + buf->used, buf->size - buf->used);
  if(result >= 0)
    buf->used += result;
  return result;
}    // From Recitation

void produce_output(ByteBuffer* buf, int n, int c, int n_case, int c_case){
  int lines_output = 0;                         //  how many lines have been outputted
  int bytes_output = 0;                         //  how many bytes have been oututted
  int num_to_output = n_case ? n : 10;          //  determines the correct number of lines to output

  for(int i = 0; i < buf->used; i++){
    if(c_case) {
      if(bytes_output < c) {
        putchar(buf->base[i]);
        bytes_output++;
      } else {
        break;
      }
    } else {
      if(lines_output < num_to_output) {
        putchar(buf->base[i]);
        if(buf->base[i] == '\n'){
          lines_output++;
        }
      } else {
        break;
      }
    }
  }
}

void new_produce_output(ByteBuffer* buf, int n, int c, int n_case, int c_case, int plus_sign) {
    int lines_output = 0;
    int bytes_output = 0;
    int started_output = !plus_sign;

    for (int i = 0; i < buf->used; i++) {
        if (plus_sign && n_case) {
            if (buf->base[i] == '\n') {
                lines_output++;
            }
            if (lines_output == n) {
                plus_sign = 0;
                started_output = 1;
                lines_output = 0;
                n = 11;
            }
            continue;
        }

        if (plus_sign && c_case) {
            bytes_output++;
            if (bytes_output >= c) {
                plus_sign = 0;
                started_output = 1;
                bytes_output = 0;
                n = 10; 
                n_case = 1; // line count switched on
                c_case = 0; // byte count switched off
            }
            
            continue;
        }

        if (started_output) {
            if (n_case) {
                putchar(buf->base[i]);
                flag_newproduce_was_called++;
                if (buf->base[i] == '\n') {
                    lines_output++;
                }
                if (lines_output >= n) {
                    break;
                }
            } else if (c_case) {
                putchar(buf->base[i]);
                flag_newproduce_was_called++;
                bytes_output++;
                if(buf->base[i] == '\n'){
                  lines_output++;
                }
                if (bytes_output >= c) {
                    break;
                }
            }
        }
    }
    if (flag_newproduce_was_called == 0) {
      produce_output(buf, n, c, n_case, c_case);
    }
}


int main(int argc, char* argv[]){
  /*
    Note: This is not starter code, this is not how you should handle the
    input file. Your code must handle files of arbitrary size.

    This code is provided only to demonstrate that you can use the same open,
    read, malloc, etc. functions as were available in xv6. With these you
    can construct your buffer handling routines.

    The only difference is that printf does not require you to pass a file
    descriptor as its first argument.


    For those unfamiliar with how to compile and run a normal C file, you can
    use the following commands:
      To compile the code:
        gcc -o head head.c
      Run the code with no arguments:
        ./head
      Run the code with an input file as argument:
        ./head input.txt
      Run the code, piping input from another program:
        cat input.txt | head
  */
  int n_case = 0;
  int c_case = 0;
  int n_value = 0;
  int c_value = 0;
  
  int option;    // which option, n or c
  int plus_sign = 0;  // indicate whether it is n, +n, c, +c

  while ((option = getopt(argc, argv, "n:c:")) != -1) {
        switch (option) {
            case 'n':
                n_case = 1;
                if (optarg[0] == '+') {
                    plus_sign = 1;
                    n_value = atoi(optarg + 1);
                } else {
                    n_value = atoi(optarg);
                }
                break;
            case 'c':
                c_case = 1;
                if (optarg[0] == '+') {
                    plus_sign = 1;
                    c_value = atoi(optarg + 1);
                } else {
                    c_value = atoi(optarg);
                }
                break;
            default:
                perror("Incorrect command-line syntax!");
                exit(1);
        }
    }

  ByteBuffer buf = allocBuffer(1024);
  int fd = 0; // allow standard input

   if (optind < argc) {
    fd = open(argv[optind], O_RDONLY);
    if (fd < 0) {
      perror("Error opening file!");
      exit(1);
    }
  }

  while ( readIntoBuffer(&buf, fd) > 0 ){
    if (buf.used == buf.size){
      reallocBuffer(&buf, 2 * buf.size);
    }
  }

  new_produce_output(&buf, n_value, c_value, n_case, c_case, plus_sign);

  if(fd != 0) {
    close(fd);
  }

  free(buf.base);
  exit(1);

}