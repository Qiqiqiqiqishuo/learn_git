SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
CC:=gcc
client_bar:$(OBJS)
	$(CC) $^ -o $@ -lpthread -lcrypt 
%.o:%.c
	$(CC) -c $^ -o $@ -g 
.PHONY:clean rebuild
clean:
	$(RM) $(OBJS) client_bar
rebuild:clean client_bar