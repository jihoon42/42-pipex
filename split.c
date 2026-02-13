#include "pipex.h"

static int	is_ws(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

static int	count_words(const char *s)
{
	int	count;

	count = 0;
	while (*s)
	{
		while (*s && is_ws(*s))
			s++;
		if (*s)
			count++;
		while (*s && !is_ws(*s))
			s++;
	}
	return (count);
}

static char	*dup_word(const char *start, size_t len)
{
	char	*word;

	word = malloc(len + 1);
	if (!word)
		return (NULL);
	memcpy(word, start, len);
	word[len] = '\0';
	return (word);
}

static void	cleanup(char **arr, int used)
{
	while (used-- > 0)
		free(arr[used]);
	free(arr);
}

char	**ft_split_ws(const char *s)
{
	char		**arr;
	int			words;
	int			i;
	const char	*start;

	if (!s)
		return (NULL);
	words = count_words(s);
	arr = malloc(sizeof(char *) * (words + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (*s)
	{
		while (*s && is_ws(*s))
			s++;
		if (!*s)
			break ;
		start = s;
		while (*s && !is_ws(*s))
			s++;
		arr[i] = dup_word(start, (size_t)(s - start));
		if (!arr[i])
			return (cleanup(arr, i), NULL);
		i++;
	}
	arr[i] = NULL;
	return (arr);
}
