#include <assert.h>
#include "compiler.h"

int E();
void STMT();
void IF();
void BLOCK();
void NO1(int, char*);
void NO2(int, char*);
void NO3(int, char*);

int tempIdx = 0, labelIdx = 0;

#define nextTemp() (tempIdx++)
#define nextLabel() (labelIdx++)
#define emit printf

int isNext(char *set) {
  char eset[SMAX], etoken[SMAX];
  sprintf(eset, " %s ", set);
  sprintf(etoken, " %s ", tokens[tokenIdx]);
  return (tokenIdx < tokenTop && strstr(eset, etoken) != NULL);
}

int isEnd() {
  return tokenIdx >= tokenTop;
}

char *next() {
  // printf("token[%d]=%s\n", tokenIdx, tokens[tokenIdx]);
  return tokens[tokenIdx++];
}

char *skip(char *set) {
  if (isNext(set)) {
    return next();
  } else {
    printf("skip(%s) got %s fail!\n", set, next());
    assert(0);
  }
}

// F = (E) | Number | Id
int F() {
  int f;
  if (isNext("(")) { // '(' E ')'
    next(); // (
    f = E();
    next(); // )
  } else { // Number | Id
    f = nextTemp();
    char *item = next();
    emit("t%d = %s\n", f, item);
  }
  return f;
}

void NO1(int f,char *item){
    if (isalpha(item[0]))
    {
        printf("@%s\n", item);
        printf("D=M\n");
    } 
    else {
        printf("@%s\n", item);
        printf("D=A\n");
    }
    printf("@t%d\n", f);
    printf("M=D\n");
}

// E = F (op E)*
int E() {
  int i1 = F();
  while (isNext("+ - * / & | ! < > =")) {
    char *op = next();
    int i2 = E();
    int i = nextTemp();
    emit("t%d = t%d %s t%d\n", i, i1, op, i2);
    i1 = i;
  }
  return i1;
}

void NO2(int i, int i1, int i2, char *op){
    printf("@t%d\n", i1);
    printf("D=M\n");
    printf("@t%d\n", i2);
    printf("D=M%sD\n", op);
    printf("@t%d\n", i);
    printf("M=D\n");
}

// ASSIGN = id '=' E;
void ASSIGN() {
  char *id = next();
  skip("=");
  int e = E();
  skip(";");
  emit("%s = t%d\n", id, e);
}

void NO3(int e, char *id){
  printf("@t%d\n", e);
  printf("D=M\n");
  printf("@%s\n", id);
  printf("M=D\n");
}

// while (E) STMT
void WHILE() {
  int whileBegin = nextLabel();
  int whileEnd = nextLabel();
  emit("(L%d)\n", whileBegin);
  skip("while");
  skip("(");
  int e = E();
  emit("if not T%d goto L%d\n", e, whileEnd);
  skip(")");
  STMT();
  emit("goto L%d\n", whileBegin);
  emit("(L%d)\n", whileEnd);
}

void IF() {
  skip("if");
  skip("(");
  int a=E();
  skip(")");
  int elsebegin=nextLabel();
  emit("if not T%d goto L%d\n", a, elsebegin);
  STMT();
  int ifend=nextLabel();
  emit("goto L%d\n", ifend);
  skip("else");
  emit("(L%d)\n", elsebegin);
  STMT();
  emit("(L%d)\n", ifend);
}

void STMT() {
  if (isNext("while"))
    return WHILE();
  else if (isNext("if"))
     IF();
  else if (isNext("{"))
    BLOCK();
  else
    ASSIGN();
}

void STMTS() {
  while (!isEnd() && !isNext("}")) {
    STMT();
  }
}

// { STMT* }
void BLOCK() {
  skip("{");
  STMTS();
  skip("}");
}

void PROG() {
  STMTS();
}

void parse() {
  printf("============ parse =============\n");
  tokenIdx = 0;
  PROG();
}