#include "pipex.h"

void	error_exit(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void	free_strs(char **strs)
{
	int	i;

	if (!strs)
		return ;
	i = 0;
	while (strs[i])
	{
		free(strs[i]);
		i++;
	}
	free(strs);
}

static char	*dup_string(const char *s)
{
	size_t	len;
	char	*dup;

	len = strlen(s);
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	memcpy(dup, s, len);
	dup[len] = '\0';
	return (dup);
}

static char	*join_cmd(char *dir, char *cmd)
{
	char	*full;
	size_t	len_dir;
	size_t	len_cmd;

	len_dir = strlen(dir);
	len_cmd = strlen(cmd);
	full = malloc(len_dir + len_cmd + 2);
	if (!full)
		return (NULL);
	memcpy(full, dir, len_dir);
	full[len_dir] = '/';
	memcpy(full + len_dir + 1, cmd, len_cmd);
	full[len_dir + len_cmd + 1] = '\0';
	return (full);
}

static int	count_parts(const char *s, char sep)
{
	int	count;

	count = 1;
	while (*s)
	{
		if (*s == sep)
			count++;
		s++;
	}
	return (count);
}

static void	cleanup_parts(char **arr, int used)
{
	while (used-- > 0)
		free(arr[used]);
	free(arr);
}

static char	**split_by_sep(const char *s, char sep)
{
	char		**arr;
	int			parts;
	int			i;
	const char	*start;

	parts = count_parts(s, sep);
	arr = malloc(sizeof(char *) * (parts + 1));
	if (!arr)
		return (NULL);
	i = 0;
	while (i < parts)
	{
		start = s;
		while (*s && *s != sep)
			s++;
		arr[i] = malloc((size_t)(s - start) + 1);
		if (!arr[i])
			return (cleanup_parts(arr, i), NULL);
		memcpy(arr[i], start, (size_t)(s - start));
		arr[i][s - start] = '\0';
		if (*s == sep)
			s++;
		i++;
	}
	arr[i] = NULL;
	return (arr);
}

char	*get_cmd_path(char *cmd, char **envp)
{
	char	**paths;
	char	*path_line;
	char	*full;
	int		i;

	if (!cmd || !*cmd)
		return (NULL);
	if (strchr(cmd, '/'))
		return (dup_string(cmd));
	while (*envp && strncmp(*envp, "PATH=", 5) != 0)
		envp++;
	if (!*envp)
		return (NULL);
	path_line = *envp + 5;
	paths = split_by_sep(path_line, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full = join_cmd(paths[i], cmd);
		if (full && access(full, X_OK) == 0)
			return (free_strs(paths), full);
		free(full);
		i++;
	}
	free_strs(paths);
	return (NULL);
}

void	exec_command(char *cmd_str, char **envp)
{
	char	**argv;
	char	*cmd_path;

	argv = ft_split_ws(cmd_str);
	if (!argv || !argv[0])
	{
		free_strs(argv);
		write(2, "pipex: command not found\n", 25);
		exit(127);
	}
	cmd_path = get_cmd_path(argv[0], envp);
	if (!cmd_path)
	{
		write(2, "pipex: command not found: ", 25);
		write(2, argv[0], strlen(argv[0]));
		write(2, "\n", 1);
		free_strs(argv);
		exit(127);
	}
	execve(cmd_path, argv, envp);
	perror(argv[0]);
	free(cmd_path);
	free_strs(argv);
	exit(126);
}
