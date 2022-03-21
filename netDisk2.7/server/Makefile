SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
CC:=gcc
server:$(OBJS)
	$(CC) $^ -o $@ -lpthread -lcrypt 
%.o:%.c
	$(CC) -c $^ -o $@ -g 
.PHONY:clean rebuild
clean:
	$(RM) $(OBJS) server 
rebuild:clean server