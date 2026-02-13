#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <stdio.h>
# include <string.h>

typedef struct s_pipex
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
}t_pipex;

void	error_exit(char *msg);
void	free_strs(char **strs);
char	**ft_split_ws(const char *s);
char	*get_cmd_path(char *cmd, char **envp);
void	exec_command(char *cmd_str, char **envp);

#endif
