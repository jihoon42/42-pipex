#include "pipex.h"

static void	child1(char **argv, char **envp, int *pipefd)
{
	int	infile;

	infile = open(argv[1], O_RDONLY);
	if (infile < 0)
		error_exit(argv[1]);
	if (dup2(infile, STDIN_FILENO) < 0)
		error_exit("dup2");
	if (dup2(pipefd[1], STDOUT_FILENO) < 0)
		error_exit("dup2");
	close(infile);
	close(pipefd[0]);
	close(pipefd[1]);
	exec_command(argv[2], envp);
}

static void	child2(char **argv, char **envp, int *pipefd)
{
	int	outfile;

	outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile < 0)
		error_exit(argv[4]);
	if (dup2(pipefd[0], STDIN_FILENO) < 0)
		error_exit("dup2");
	if (dup2(outfile, STDOUT_FILENO) < 0)
		error_exit("dup2");
	close(outfile);
	close(pipefd[0]);
	close(pipefd[1]);
	exec_command(argv[3], envp);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	p;
	int		status2;

	if (argc != 5)
	{
		write(2, "Usage: ./pipex file1 cmd1 cmd2 file2\n", 37);
		return (1);
	}
	if (pipe(p.pipefd) < 0)
		error_exit("pipe");
	p.pid1 = fork();
	if (p.pid1 < 0)
		error_exit("fork");
	if (p.pid1 == 0)
		child1(argv, envp, p.pipefd);
	p.pid2 = fork();
	if (p.pid2 < 0)
		error_exit("fork");
	if (p.pid2 == 0)
		child2(argv, envp, p.pipefd);
	close(p.pipefd[0]);
	close(p.pipefd[1]);
	waitpid(p.pid1, NULL, 0);
	waitpid(p.pid2, &status2, 0);
	if (WIFEXITED(status2))
		return (WEXITSTATUS(status2));
	return (1);
}
