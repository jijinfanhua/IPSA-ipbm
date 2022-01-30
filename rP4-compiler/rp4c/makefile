LEX      = flex
YACC     = bison
CC       = g++
CFLAGS   = -std=gnu++2a
H_RP4    = $(wildcard ./rp4_*.h) $(wildcard ./*.hpp)
H_IPSA   = $(wildcard ./ipsa_*.h)
SOURCE   = rp4c.cpp
TARGET   = rp4c
OBJECT   = rp4c.o
CMP_SRC  = rp4_compiler.cpp
CMP_TGT  = rp4_compiler
CMP_OBJ  = rp4_compiler.o
LEX_OBJ  = lex.yy.o
LEX_SRC  = lex.l
LEX_C    = lex.yy.c
YACC_OBJ = yacc.o
YACC_SRC = yacc.y
YACC_CPP = yacc.cpp

all: ${YACC_OBJ} ${LEX_OBJ} ${CMP_OBJ} ${OBJECT}
	${CC} ${CFLAGS} ${LEX_OBJ} ${YACC_OBJ} ${OBJECT} -o ${TARGET}
	${CC} ${CFLAGS} ${LEX_OBJ} ${YACC_OBJ} ${CMP_OBJ} -o ${CMP_TGT}

${YACC_OBJ}: ${YACC_SRC} ${H_RP4}
	${YACC} -d ${YACC_SRC} -o ${YACC_CPP}
	${CC} ${CFLAGS} -c ${YACC_CPP}

${LEX_OBJ}: ${LEX_SRC} ${H_RP4}
	${LEX} ${LEX_SRC}
	${CC} ${CFLAGS} -c ${LEX_C}

${CMP_OBJ}: ${CMP_SRC} ${H_IPSA} ${H_RP4}
	${CC} ${CFLAGS} -c ${CMP_SRC}

${OBJECT}: ${SOURCE} ${H_IPSA} ${H_RP4}
	${CC} ${CFLAGS} -c ${SOURCE}

clean:
	-rm *.o
	-rm ${TARGET}