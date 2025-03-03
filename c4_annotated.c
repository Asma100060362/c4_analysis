// c4.c - A super tiny C compiler written in C
// This program is a simple compiler that can read basic C code and execute it.
// It works by translating C code into a set of instructions that it can run on a virtual machine.
// Let's go through the code step by step!

#include <stdio.h>   // Standard input-output library
#include <stdlib.h>  // Standard library for memory allocation, process control, etc.
#include <memory.h>  // Used for memory manipulation functions
#include <unistd.h>  // POSIX standard functions for low-level file operations
#include <fcntl.h>   // File control options

// Using 'int' as 'long long' to handle large values easily
#define int long long

// Pointers to keep track of different parts of the source code and compiled code
char *p, *lp,  // 'p' is the current position in the source code, 'lp' keeps track of the line start
     *data;    // Pointer for storing variables and other data

// Integer pointers for managing compiled code and identifiers
int *e, *le,  // 'e' points to the generated instructions, 'le' is the last emitted instruction
    *id,      // Points to the current identifier being processed
    *sym,     // Symbol table for storing variable/function names
    tk,       // Current token being processed
    ival,     // Value of the current token if it's a number
    ty,       // Type of the current expression (int, char, pointer)
    loc,      // Keeps track of local variables in functions
    line,     // Current line number (for debugging/error messages)
    src,      // Flag to print the source code as it's being compiled
    debug;    // Flag to print executed instructions

// Token types (These are different kinds of things we can find in the code: numbers, keywords, etc.)
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// These are the operation codes (opcodes) for the instructions the compiler generates
// They are used by the virtual machine to execute the compiled program
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

// Variable types (char, int, pointer)
enum { CHAR, INT, PTR };

// Structure of an identifier (since we don't have a full struct, we use an array)
enum { Tk, Hash, Name, Class, Type, Val, HClass, HType, HVal, Idsz };

// Function to get the next token in the source code
void next() {
  char *pp;
  
  while (tk = *p) {  // Keep going while we have characters in the source code
    ++p;
    if (tk == '\n') {  // If we find a newline
      line++;  // Increase line count
      lp = p;  // Update line start pointer
    }
    else if (tk == '#') {  // If it's a preprocessor directive
      while (*p != 0 && *p != '\n') ++p;
    }
    else if ((tk >= 'a' && tk <= 'z') || (tk >= 'A' && tk <= 'Z') || tk == '_') {
      // This part handles identifiers (variable and function names)
      pp = p - 1;
      while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')
        tk = tk * 147 + *p++;
      tk = (tk << 6) + (p - pp);
      id = sym;
      while (id[Tk]) {
        if (tk == id[Hash] && !memcmp((char *)id[Name], pp, p - pp)) { tk = id[Tk]; return; }
        id = id + Idsz;
      }
      id[Name] = (int)pp;
      id[Hash] = tk;
      tk = id[Tk] = Id;
      return;
    }
    else if (tk >= '0' && tk <= '9') {
      
      if (ival = tk - '0') { while (*p >= '0' && *p <= '9') ival = ival * 10 + *p++ - '0'; }
      else if (*p == 'x' || *p == 'X') {
        while ((tk = *++p) && ((tk >= '0' && tk <= '9') || (tk >= 'a' && tk <= 'f') || (tk >= 'A' && tk <= 'F')))
          ival = ival * 16 + (tk & 15) + (tk >= 'A' ? 9 : 0);
      }
      else { while (*p >= '0' && *p <= '7') ival = ival * 8 + *p++ - '0'; }
      tk = Num;
      return;
    }
    else if (tk == '/') {
      if (*p == '/') {  
        ++p;
        while (*p != 0 && *p != '\n') ++p;
      }
      else {
        tk = Div;
        return;
      }
    }
  }
}
