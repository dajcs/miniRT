/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_splits.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42luxembourg.lu>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:11:05 by anemet            #+#    #+#             */
/*   Updated: 2025/10/02 14:30:51 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anemet <anemet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 14:31:45 by anemet            #+#    #+#             */
/*   Updated: 2025/04/02 15:35:19 by anemet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	is_sep(char c, char *charset)
{
	char	*p;

	p = charset;
	while (*p)
	{
		if (c == *p)
			return (1);
		p++;
	}
	return (0);
}

char	**malloc_res(char *str, char *charset)
{
	int		count;
	int		in_word;
	char	**res;

	count = 0;
	in_word = 0;
	while (*str)
	{
		if (!is_sep(*str, charset) && in_word == 0)
		{
			count++;
			in_word = 1;
		}
		else if (is_sep(*str, charset))
			in_word = 0;
		str++;
	}
	res = malloc(sizeof(char *) * (count + 1));
	return (res);
}

char	*copy_word(char *str, int start, int end)
{
	int		len;
	int		i;
	char	*word;

	len = end - start;
	word = malloc(sizeof(char) * (len + 1));
	if (!word)
		return (0);
	i = 0;
	while (i < len)
	{
		word[i] = str[start + i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

// while loop:
// skip leading or consecutive separators
// if arrived to end of str break loop
// mark start of token (start = i)
// find end of token (while (str[i] && !is_sep(str[i], charset)))
// extract token
char	**ft_splits(char *str, char *charset)
{
	int		i;
	int		index;
	int		start;
	char	**res;

	res = malloc_res(str, charset);
	if (!res)
		return (0);
	i = 0;
	index = 0;
	while (str[i])
	{
		while (str[i] && is_sep(str[i], charset))
			i++;
		if (!str[i])
			break ;
		start = i;
		while (str[i] && !is_sep(str[i], charset))
			i++;
		res[index] = copy_word(str, start, i);
		index++;
	}
	res[index] = 0;
	return (res);
}
