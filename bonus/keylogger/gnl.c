/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misaev <misaev@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/14 13:44:31 by misaev            #+#    #+#             */
/*   Updated: 2021/05/17 10:07:08 by misaev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"

int	ft_strlen_line(const char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\n' && str[i] != '\0')
		i++;
	return (i);
}

int	check_line(char **save, char **line)
{
	int		i;
	char	*temp;

	i = ft_strlen_line(*save);
	if ((*save)[i] == '\n')
	{
		*line = ft_substr(*save, 0, i);
		temp = ft_strdup(*save + i + 1);
		free(*save);
		*save = temp;
	}
	else
	{
		*line = ft_strdup(*save);
		free(*save);
		*save = NULL;
		return (0);
	}
	return (1);
}

int	protection(char **save, char **line, int byte_lu)
{
	if (byte_lu < 0)
		return (-1);
	if (*save == NULL && byte_lu == 0)
	{
		*line = ft_strdup("");
		return (0);
	}
	else
		return (check_line(save, line));
}

int	get_next_line(int fd, char **line)
{
	char			buf[BUFFER_SIZE + 1];
	int				byte_lu;
	static char		*save;
	char			*temp;

	if (fd < 0 || line == NULL || read(fd, buf, 0) < 0)
		return (-1);
	byte_lu = read(fd, buf, BUFFER_SIZE);
	while (byte_lu > 0)
	{
		buf[byte_lu] = '\0';
		if (save == NULL)
			save = ft_strdup("");
		temp = ft_strjoin(save, buf);
		free(save);
		save = temp;
		if (ft_strchr(buf, '\n'))
			break ;
		byte_lu = read(fd, buf, BUFFER_SIZE);
	}
	return (protection(&save, line, byte_lu));
}
