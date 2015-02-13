CHARMDIR = /home/work/Code/charm
OPTS=-O3 -optimize -production
CHARMC = $(CHARMDIR)/bin/charmc $(OPTS)


default: all
all: spmv


spmv : Main.o RowSlice.o Row.o
	$(CHARMC) -language charm++ -o spmv Main.o RowSlice.o Row.o

Main.o : Main.C Main.h main.decl.h main.def.h row_slice.decl.h row.decl.h
	$(CHARMC) -o Main.o Main.C

main.decl.h main.def.h : Main.ci
	$(CHARMC) Main.ci
	
RowSlice.o : RowSlice.C RowSlice.h row_slice.decl.h row_slice.def.h
	$(CHARMC) -o RowSlice.o RowSlice.C
	
row_slice.decl.h row_slice.def.h : RowSlice.ci
	$(CHARMC) RowSlice.ci
	
Row.o : Row.C Row.h row.decl.h row.def.h main.decl.h
	$(CHARMC) -o Row.o Row.C
	
row.decl.h row.def.h : Row.ci
	$(CHARMC) Row.ci
	
clean:
	rm -f main.decl.h main.def.h Main.o
	rm -f row.decl.h row.def.h Row.o
	rm -f row_slice.decl.h row_slice.def.h RowSlice.o
	rm -f row.decl.h row.def.h Row.o
	rm -f spmv charmrun
