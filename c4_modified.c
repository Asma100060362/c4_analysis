
// c4_modified.c - Modified C4 Compiler with Modulus (%) Operator

// Modifications:
// - Added support for the '%' (modulus) operator.
// - Updated the tokenizer, parser, and virtual machine execution.

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#define int long long

char *p, *lp, *data; // Source pointer and memory storage

int *e, *le, *id, *sym, tk, ival, ty, loc, line, src, debug;

// Tokens and Operator Precedence (Added 'Mod' for modulus)
enum { Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak };

// Virtual Machine Instructions (Added 'MOD' for modulus operation)
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD,
       OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

void next() {
  char *pp;
  while (tk = *p) {
    ++p;
    if (tk == '
') { ++line; }
    else if (tk == '#') { while (*p != 0 && *p != '
') ++p; }
    else if ((tk >= 'a' && tk <= 'z') || (tk >= 'A' && tk <= 'Z') || tk == '_') {
      pp = p - 1;
      while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_') tk = tk * 147 + *p++;
      tk = (tk << 6) + (p - pp);
      id = sym;
      while (id[0]) { if (tk == id[1] && !memcmp((char *)id[2], pp, p - pp)) { tk = id[0]; return; } id = id + 10; }
      id[2] = (int)pp; id[1] = tk; tk = id[0] = Id; return;
    }
    else if (tk >= '0' && tk <= '9') {
      if (ival = tk - '0') { while (*p >= '0' && *p <= '9') ival = ival * 10 + *p++ - '0'; }
      tk = Num; return;
    }
    else if (tk == '*') { tk = Mul; return; }
    else if (tk == '%') { tk = Mod; return; } // Added support for modulus operator
  }
}

void expr(int lev) {
  int t, *d;
  if (!tk) { printf("%d: unexpected eof in expression
", line); exit(-1); }
  else if (tk == Num) { *++e = IMM; *++e = ival; next(); ty = INT; }
  else if (tk == '(') { next(); expr(Assign); if (tk == ')') next(); else { printf("%d: close paren expected
", line); exit(-1); } }
  else { printf("%d: bad expression
", line); exit(-1); }

  while (tk >= lev) {
    t = ty;
    if (tk == Mul) { next(); *++e = PSH; expr(Inc); *++e = MUL; ty = INT; }
    else if (tk == Div) { next(); *++e = PSH; expr(Inc); *++e = DIV; ty = INT; }
    else if (tk == Mod) { next(); *++e = PSH; expr(Inc); *++e = MOD; ty = INT; } // Added modulus operation
  }
}

int main() {
  int *pc, *sp, *bp, a, cycle;
  pc = e; sp = (int *)((int)malloc(256 * 1024) + (256 * 1024));
  bp = sp; cycle = 0;
  
  while (1) {
    int i = *pc++;
    if (i == IMM) a = *pc++;
    else if (i == ADD) a = *sp++ + a;
    else if (i == SUB) a = *sp++ - a;
    else if (i == MUL) a = *sp++ * a;
    else if (i == DIV) a = *sp++ / a;
    else if (i == MOD) a = *sp++ % a; // Added modulus execution support
    else if (i == EXIT) { printf("exit(%d) cycle = %d
", *sp, cycle); return *sp; }
  }
}
