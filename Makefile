# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/02 13:34:30 by anemet            #+#    #+#              #
#    Updated: 2025/10/02 20:07:59 by anemet           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = miniRT

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
				src/math/vec3_ops2.c \
				src/math/matrix_ops.c \
				src/math/ray_ops.c

SRCS_RENDER = src/render/render.c \
				src/render/ray.c \
				src/render/intersections.c \
				src/render/intersect_sphere.c \
				src/render/intersect_plane.c \
				src/render/intersect_cylinder.c \
				src/render/intersect_cone.c \
				src/render/lighting.c \
				src/render/shadows.c \
				src/render/colors.c \
				src/render/camera.c \
				src/render/camera_utils.c \
				src/render/scene_objects.c

# Test-specific files
SRCS_TEST = src/main.c \
            src/render/mock_render.c

# Combine all source files
SRCS = $(SRCS_PARSER) $(SRCS_WINDOW) $(SRCS_TEST)

# Object files
OBJS = $(SRCS:.c=.o)

# Header include paths
INCLUDES = -I./include -I$(LIBFT_DIR) -I$(MLX_DIR)

# ------- Rules ----------

# Default rule
all: $(NAME)

# The bonus rule -- same as 'all'
bonus: $(NAME)

# Rule to create the final executable
$(NAME): $(OBJS) $(LIBFT) $(MLX_LIB)
	@echo "Linking $@..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
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
	@echo "Object files cleaned."

fclean: clean
	@make fclean -C $(LIBFT_DIR)
	@rm -f $(NAME)
	@echo "Full clean complete."

re: fclean all

.PHONY: all clean fclean re
