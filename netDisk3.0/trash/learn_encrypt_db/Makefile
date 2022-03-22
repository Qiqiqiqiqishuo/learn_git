SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
CC:=gcc
main:$(OBJS)
	$(CC) $^ -o $@ -lpthread -lcrypt -lmysqlclient
%.o:%.c
	$(CC) -c $^ -o $@ -g 
.PHONY:clean rebuild
clean:
	$(RM) $(OBJS) main 
rebuild:clean main
