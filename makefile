#Look at http://www.physics.irfu.se/aCCdoc/libs.htm#shlib
# to see a reference on how to create libraries

#Importante - para rodar o programa, é necessário executar a linha abaixo primeiro
#export SHLIB_PATH=$ORACLE_HOME/lib32

# ###############################################################
# The default option needs to be the first one
default: all
# ###############################################################

# ###############################################################
# ORACLE
# Include necessario para que o ProC opere funcionamento
# ------------------------------------------------------
include $(ORACLE_HOME)/precomp/lib/env_precomp.mk
ORACLE_FLAGS=$(CFLAGS64)
#LDFLAGS32=-Wl,+s -Wl,+n +DAportable +DS2.0 -L$(ORACLE_HOME)/precomp//lib32/ -L$(ORACLE_HOME)/lib32/
ORACLE_LIBS= -Wl,+k -Wl,+s -Wl,+n +DA2.0W +DS2.0 -L/tools/oracle/11.1.0/precomp/lib/ -L/tools/oracle/11.1.0/lib/ -Wl,-a,default -lsql11 `cat $(ORACLE_HOME)/lib/ldflags` -locijdbc11
#$(LDFLAGS)
# -Wl,-a,default -lsql11 `cat $(ORACLE_HOME)/lib/ldflags` -locijdbc11_g
PROC_EXEC= $(PROC_64)
PROCFLAGS=ORACA=YES THREADS=YES include=. MODE=ANSI DYNAMIC=ANSI TYPE_CODE=ANSI CODE=ANSI_C CLOSE_ON_COMMIT=YES 

# ###############################################################


# ###############################################################
# PATHs
BASE_PATH=..
BIN_PATH=$(BASE_PATH)/bin
# ###############################################################

# ###############################################################
# Binarios criados
LIBNAME=scclib_db
BIN=lib$(LIBNAME).sl
C=
PC=scclib_db.pc

BIN_TEST=sccdb_test
C_TEST=test.c
PC_TEST=
# ###############################################################

# ###############################################################
# Flags de compilacao
CC=cc
#+z 
CFLAGS_ALL=+z -g -Wall -AC99 $(ORACLE_FLAGS) 
LDFLAGS_LIB=-b $(ORACLE_LIBS)  
#-b 
LDFLAGS_TEST=$(ORACLE_LIBS) -L$(BIN_PATH) -l$(LIBNAME)
# ###############################################################

# ###############################################################
# Regras de compilacao
# --------------------

.SUFFIXES: .o .c .pc

.c.o:
	@echo ""
	@echo ".........................Compilando Arquivo .C Comum"
	$(CC) -c -o $@ $< $(CFLAGS_ALL)

.pc.c:
	$(PROC_EXEC) $(PROCFLAGS) iname=$*.pc oname=$*.c

.pc.o:
	@echo ""
	@echo ".........................Precompilando os fontes do Pro*C"
	$(PROC_EXEC) $(PROCFLAGS) iname=$*.pc
	@echo ""
	@echo ".........................Compilando Objeto do Pro*C"
	$(CC) $(CFLAGS_ALL) $(ORACLE_FLAGS) -c $*.c -o $*.o

SRC= ${PC:.pc=.c}
OBJS= ${SRC:.c=.o} ${C:.c=.o}
SRC_TEST= ${PC_TEST:.pc=.c}
OBJS_TEST= ${SRC_TEST:.c=.o} ${C_TEST:.c=.o}
# ###############################################################

# ###############################################################
# Tags
all: clean scclibdb test

scclibdb: $(OBJS)
	@echo
	@echo "====================== Compilando '$(BIN)' ======================"
	$(CC) $(OBJS) -o $(BIN_PATH)/$(BIN) $(LDFLAGS_LIB)
	rm -f tp*
	ctags -R ..
	
test: $(OBJS_TEST)
	@echo
	@echo "=================== Compilando '$(BIN_TEST)' ===================="
	$(CC) $(OBJS_TEST) -o $(BIN_PATH)/$(BIN_TEST) $(LDFLAGS_TEST)
	rm -f tp*
	ctags -R ..

clean:
	@echo
	@echo "====================== Limpeza ======================"
	-rm -f $(BIN_PATH)/$(BIN_TEST)
	-rm -f $(BIN_PATH)/$(BIN)	
	-rm scclib_db.c 
	-rm -f *.o *.bak *.lis tp*
# ###############################################################

