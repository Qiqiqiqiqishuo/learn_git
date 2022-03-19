#SRCS:=$(wildcard *.c)
#OBJS:=$(SRCS:%.c=%.o)
#CC:=gcc
#server:$(OBJS)
#	$(CC) $^ -o $@ -lpthread
#%.o:%.c
#	$(CC) -c $^ -o $@ -g 
#.PHONY:clean rebuild
#clean:
#	$(RM) $(OBJS) server 
#rebuild:clean server

SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
CC:=gcc
server:$(OBJS)
	$(CC) $^ -o $@ -lpthread
%.o:%.c
	$(CC) -c $^ -o $@ -g 
.PHONY:clean rebuild
clean:
	$(RM) $(OBJS) server 
rebuild:clean server

# CC:=gcc
# OUT:=main
# #HEADER:=header
# SRCS:=$(wildcard *.c)
# OBJS:=$(SRCS:%.c=%.o)
# $(OUT):$(OBJS)
# 	$(CC) $^ -o $@
# %.o:%.c
# 	$(CC) -c $^ -o $@ 
# #-I $(HEADER)
# .PHONY: clean rebuild
# clean:
# 	$(RM) $(OUT) $(OBJS)
# rebuild:clean $(OUT)