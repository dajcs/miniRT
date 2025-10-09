# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/02 13:34:30 by anemet            #+#    #+#              #
#    Updated: 2025/10/09 15:07:53 by anemet           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = miniRT
NAME_BONUS = miniRTbonus

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
# libft & minilibx
MLX_DIR = ./minilibx/
LIBFT_DIR = ./libft/
MLX_LIB = $(MLX_DIR)libmlx.a
LIBFT = $(LIBFT_DIR)libft.a
LDFLAGS = -L$(LIBFT_DIR) -lft -L$(MLX_DIR) -lmlx -lXext -lX11 -lm

# Source files
SRCS_PARSER = src/parser/parser.c \
                src/parser/parser_utils.c \
                src/parser/parser_elements.c \
                src/parser/parser_element2.c \
				src/parser/parser_validation.c \
                src/parser/errors.c

SRCS_WINDOW = src/window/window.c \
                src/window/hooks.c \
                src/window/cleanup.c

SRCS_MATH = src/math/vec3_ops1.c \
				src/math/vec3_ops2.c

SRCS_RENDER = src/render/renderer.c \
				src/render/camera.c \
				src/render/cylinder_intersect.c \
				src/render/intersections.c \
				src/render/lighting.c

### Bonus Source Files ###
# Source files
SRCS_PARSER_BONUS = src/parser/parser.c \
                src/parser/parser_utils.c \
                src/parser/parser_elements_bonus.c \
                src/parser/parser_element2_bonus.c \
				src/parser/parser_validation.c \
                src/parser/errors.c

SRCS_WINDOW_BONUS = src/window/window.c \
                src/window/hooks.c \
                src/window/cleanup.c

SRCS_MATH_BONUS = src/math/vec3_ops1.c \
				src/math/vec3_ops2.c \
				src/math/vec3_ops3_bonus.c \

SRCS_RENDER_BONUS = src/render/renderer.c \
				src/render/camera.c \
				src/render/cylinder_intersect.c \
				src/render/intersections.c \
				src/render/lighting_bonus.c


# Combine all source files
SRCS = $(SRCS_PARSER) $(SRCS_WINDOW) $(SRCS_RENDER) $(SRCS_MATH) src/main.c

### Combine all Bonus source files ###
SRCS_BONUS = $(SRCS_PARSER_BONUS) $(SRCS_WINDOW_BONUS) $(SRCS_RENDER_BONUS) $(SRCS_MATH_BONUS) src/main.c

# Object files
OBJS = $(SRCS:.c=.o)

### Bonus Object files ###
OBJS_BONUS = $(SRCS_BONUS:.c=.o)

# Header include paths
INCLUDES = -I./include -I$(LIBFT_DIR) -I$(MLX_DIR)

# ------- Rules ----------

# Default rule
all: $(NAME)

# The bonus rule -- same as 'all'
bonus: $(NAME_BONUS)

# Rule to create the final executable
$(NAME): $(OBJS) $(LIBFT) $(MLX_LIB)
	@echo "Linking $@..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "$@ compiled successfully!"

### Rule to create the final Bonus executable ###
$(NAME_BONUS): $(OBJS_BONUS) $(LIBFT) $(MLX_LIB)
	@echo "Linking $@..."
	@$(CC) $(CFLAGS) $(OBJS_BONUS) -o $(NAME_BONUS) $(LDFLAGS)
	@echo "$@ compiled successfully!"

# Rule to build the libft library
$(LIBFT):
	@echo "Compiling libft..."
	@$(MAKE) -s -C $(LIBFT_DIR)

# Rule to build the MiniLibX library
$(MLX_LIB):
	@echo "Compiling MiniLibX..."
	@$(MAKE) -s -C $(MLX_DIR)

# Compile source files to object files
%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Standard rules
clean:
	@make clean -C $(LIBFT_DIR)
	@make clean -C $(MLX_DIR)
	@rm -f $(OBJS)
	@rm -f $(OBJS_BONUS)
	@echo "Object files cleaned."

fclean: clean
	@make fclean -C $(LIBFT_DIR)
	@rm -f $(NAME)
	@rm -f $(NAME_BONUS)
	@echo "Full clean complete."

re: fclean all

.PHONY: all clean fclean re
