/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misaev <misaev@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/13 10:52:50 by hkrifa            #+#    #+#             */
/*   Updated: 2021/05/17 10:09:07 by misaev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10
# endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/select.h>
#include <errno.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int		get_next_line(int fd, char **line);
char	*ft_strjoin(char const *s1, char const *s2);
size_t	ft_strlen(const char *str);
char	*ft_strdup(const char *src);
char	*ft_substr(char *s, unsigned int start, size_t len);
char	*ft_strchr(const char *str, int c);
#endif
