
NAME = ft_malcolm

SRCS = ./src/main.c

FLAGS = -Wall -Wextra -Werror

all:$(SRCS)
		gcc $(FLAGS) -I./include $(SRCS) -o $(NAME)

clean:rm -rf *.o

fclean :clean
			rm -rf $(NAME)

re : fclean all

