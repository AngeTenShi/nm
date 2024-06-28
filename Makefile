NAME		=	ft_nm
CC			=	gcc -g
RED			=	\033[0;31m
CYAN		=	\033[0;36m
COLOR_OFF	=	\033[0m\0
YELLOW		=	\033[0;33m
FLAGS		=	-Wall -Werror -Wextra
LFT			=	libft/libft.a
INC			=	-I ./libft
LIB			=	-L ./libft -lft
SRCS_DIR	=	srcs
OBJS_DIR	=	objs
SRCS			=	nm.c
 
OBJS		=	$(SRCS:%.c=$(OBJS_DIR)/%.o)

all:	$(NAME)

$(NAME):	$(LFT) $(OBJS)
			$(CC) $(FLAGS) -o  $@ $^ $(LIB)

$(LFT):
			@echo
			@make -s -C libft
			@echo "$(CYAN)[**] LIBFT READY !"

$(OBJS): $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
			@mkdir -p $(@D)
			@echo
			@echo "$(RED)[**] Compiling $< [**]"
			@echo "$(COLOR_OFF)"
			$(CC) $(FLAGS) $(INC) -o $@ -c $<

clean:
			@make -s $@ -C libft
			@rm -rf $(OBJS_DIR)
			@echo "$(YELLOW)Objects Files has been removed."

fclean:		clean
			@make -s $@ -C libft
			@rm -rf $(NAME)
			@echo "$(YELLOW)Binary file has been removed."

re:			fclean all

.PHONY:		all clean fclean r