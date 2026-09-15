# ====== Standard ======
NAME 				= codexion
CC 					= cc
CFLAGS 				= -Wall -Wextra -Werror -pthread
RM 					= rm
RFLAGS				= -rf

# ====== Directories ======
INC_DIR				= include/
SRCS_DIR			= srcs/
BUILD_DIR			= build/
PARSE_DIR			= ${SRCS_DIR}parse/
INIT_DIR			= ${SRCS_DIR}init/
THREAD_DIR			= ${SRCS_DIR}thread/

# ======= Includes =======
INC					= -I ${INC_DIR}

# ====== Source Files ======
PARSE				= 	${PARSE_DIR}parse.c \
						${PARSE_DIR}parse_utils.c

INIT				= 	${INIT_DIR}init.c \
						${INIT_DIR}init_utils.c

THREAD				= 	${THREAD_DIR}thread.c \
						${THREAD_DIR}thread_utils.c

SRCS 				=	$(SRCS_DIR)main.c \
     					$(SRCS_DIR)cleanup.c \
     					$(SRCS_DIR)time.c \
     					$(SRCS_DIR)coder.c \
     					$(SRCS_DIR)log.c \
     					$(SRCS_DIR)monitor.c \
						$(SRCS_DIR)dongle.c \
     					$(PARSE) \
     					$(INIT) \
						$(THREAD)

BUILD				=	$(SRCS:$(SRCS_DIR)%.c=$(BUILD_DIR)%.o)

# ====== Rules ======
all: ${NAME}

$(NAME):			$(BUILD)
						$(CC) $(CFLAGS) $(BUILD) -o $(NAME)

$(BUILD_DIR)%.o:	$(SRCS_DIR)%.c
					mkdir -p $(@D)
					$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
						$(RM) $(RFLAGS) $(BUILD_DIR)

fclean: 			clean
						${RM} $(RFLAGS) $(NAME)

re: 				fclean all

.PHONY: 			all clean fclean re