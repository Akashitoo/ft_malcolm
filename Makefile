
NAME = ft_malcolm

FLAGS = -Wall -Wextra -Werror

SRCS = ./src/main.c \
		./src/utils.c \

OBJS = $(SRCS:.c=.o)

%.o: %.c
	gcc $(FLAGS) -I./include -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	gcc $(FLAGS) -I./include $(OBJS) -o $(NAME) ./include/libft.a -g

clean:
	rm -rf ./src/*.o

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re 