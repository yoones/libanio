##
##
##
##

CC		=	gcc
RM		=	rm -rf
AR		=	ar
DOXYGEN		=	doxygen

INCLUDES	=	-Iinclude -I.
CFLAGS		+=	-W -Wall -Wextra $(INCLUDES)
LDFLAGS		=	-lpthread

## Library
NAME		=	libanio.a
SRCS		=	src/list.c			\
			src/init.c			\
			src/is_server_alive.c		\
			src/set_callback_on_accept.c	\
			src/set_callback_on_read.c	\
			src/set_callback_on_eof.c	\
			src/set_callback_on_error.c	\
			src/set_callbacks.c		\
			src/set_max_clients.c		\
			src/set_thread_pool_size.c	\
			src/start_monitor.c		\
			src/stop_monitor.c		\
			src/free.c			\
			src/create_workers.c		\
			src/destroy_workers.c		\
			src/pthread_helpers.c		\
			src/epoll_helpers.c		\
			src/fdesc_init.c		\
			src/fdesc_close.c		\
			src/has_client.c		\
			src/add_client.c		\
			src/remove_client.c		\
			src/get_client.c
OBJS		=	$(SRCS:.c=.o)

## Debug binary
NAME_DEBUG	=	poc
SRCS_DEBUG	=	$(NAME)			\
			debug/main.c
OBJS_DEBUG	=	$(SRCS_DEBUG:.c=.o)

## Rules

all		:	$(NAME) $(NAME_DEBUG)

$(NAME)		:	$(OBJS)
			$(AR) rcs $(NAME) $(OBJS)

$(NAME_DEBUG)	:	$(NAME) $(OBJS_DEBUG)
			$(CC) -o $(NAME_DEBUG) $(OBJS_DEBUG) $(NAME) $(LDFLAGS)

clean		:
			$(RM) $(NAME)
			$(RM) $(NAME_DEBUG)

fclean		:	clean
			$(RM) $(OBJS)
			$(RM) $(OBJS_DEBUG)

re		:	fclean all

.PHONY		:	all clean fclean re
