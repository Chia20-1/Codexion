# ====== Standard ======
NAME 				= codexion
CC 					= cc
CFLAGS 				= -Wall -Wextra -Werror -pthread
RM 					= rm
RFLAGS				= -rf

# ====== Directories ======
INC_DIR				= include/
SRCS_DIR			= srcs/
OBJS_DIR			= objs/
PARSE_DIR			= ${SRCS_DIR}parse/
INIT_DIR			= ${SRCS_DIR}init/

# ======= Includes =======
INC					= -I ${INC_DIR}

# ====== Source Files ======
PARSE				= 	${PARSE_DIR}parse.c \
						${PARSE_DIR}parse_utils.c

INIT				= 	${INIT_DIR}init.c \
						${INIT_DIR}init_utils.c


# ====== Rules ======
all: ${NAME}

$(NAME):			$(OBJS)
						$(CC) $(CFLAGS) $(INC) -o $(NAME)

$(OBJS_DIR)%.o:		$(SRCS_DIR)%.c
					mkdir -p $(@D)
					$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
						$(RM) $(RFLAGS) $(OBJS_DIR)

fclean: 			clean
						${RM} $(RFLAGS) $(NAME)

re: 				fclean all

.PHONY: 			all clean fclean re