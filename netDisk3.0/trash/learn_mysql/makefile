SRCS:=$(wildcard *.c)
EXES:=$(SRCS:%.c=%)
all:$(EXES)
%:%.c
	gcc $^ -o $@ -lmysqlclient
.PHONY:clean rebuild
clean:
	$(RM) $(EXES)
rebuild: clean all
