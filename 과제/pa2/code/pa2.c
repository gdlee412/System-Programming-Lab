#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>

char *executable[] = {"ls", "man", "grep", "sort", "awk", "bc"};
char *my_execs[] = {"head", "tail", "cat", "cp", "mv", "rm", "pwd"};
char *builtin_cmd[] = {"cd", "exit"};
char *path = "./";

void signal_handler(int sig);
int verify_commandline(char *cmd[50], int cmd_count);
int check_type(char *cmd);
void exec_cmd(char* cmd);

int main()
{
	size_t size;
	int fd1, fd2;
	int retval;
	char *input;
	char *cmd[50];

	signal(SIGINT, signal_handler);
        signal(SIGTSTP, signal_handler);

	while(1)
	{
		int cmd_count = 0;
		char *file1, *file2;
        	int file1exist = 0, file2exist = 0;
		
		input = NULL;
		printf("$ ");
		getline(&input, &size, stdin);
		input[strlen(input) - 1] = '\0';

		//finds number of commands
		char *ptr = strtok(input, "|");
		while(ptr != NULL)
		{
			if(cmd_count > 0)
			{
				cmd[cmd_count++] = ptr + 1;	
			}
			else
			{
				cmd[cmd_count++] = ptr;
			}
			ptr = strtok(NULL, "|");
			
			if(ptr != NULL)
			{
				 cmd[cmd_count - 1][strlen(cmd[cmd_count - 1]) - 1] = '\0';
			}
		}
		
		//check input direction
		if(strchr(cmd[0], '<') != NULL)
		{
			char *ptr = strtok(cmd[0], "<");
			cmd[0] = ptr;
			cmd[0][strlen(cmd[0]) - 1] = '\0';
			ptr = strtok(NULL, "<");
			file1 = ptr + 1;
			file1exist = 1;
		
			//check output directions for no pipeline case
			if(cmd_count == 1)
			{
				if(strchr(file1, '>') != NULL && strstr(file1, ">>") == NULL)
				{
					char *ptr = strtok(file1, ">");
					file1 = ptr;
					file1[strlen(file1) - 1] = '\0';
					ptr = strtok(NULL, ">");
					file2 = ptr + 1;
					file2exist = 1;
				}
				else if(strchr(file1, '>') != NULL && strstr(file1, ">>") != NULL)
				{
					char *ptr = strtok(file1, ">");
					file1 = ptr;
					file1[strlen(file1) - 1] = '\0';
					ptr = strtok(NULL, ">");
					file2 = ptr + 1;
					file2exist = 2;
				}
			}
		}
		//check output directions
		if(strchr(cmd[cmd_count - 1], '>') != NULL && strstr(cmd[cmd_count - 1], ">>") == NULL)
		{
			char *ptr = strtok(cmd[cmd_count - 1], ">");
			cmd[cmd_count - 1] = ptr;
			cmd[cmd_count - 1][strlen(cmd[cmd_count - 1]) - 1] = '\0';
			ptr = strtok(NULL, ">");
			file2 = ptr + 1;
			file2exist = 1;
		}
		else if(strchr(cmd[cmd_count - 1], '>') != NULL && strstr(cmd[cmd_count - 1], ">>") != NULL)
		{
			char *ptr = strtok(cmd[cmd_count - 1], ">");
			cmd[cmd_count - 1] = ptr;
			cmd[cmd_count - 1][strlen(cmd[cmd_count - 1]) - 1] = '\0';
			ptr = strtok(NULL, ">");
			file2 = ptr + 1;
			file2exist = 2;
		}

		
		if(!verify_commandline(cmd, cmd_count))
		{
			fprintf(stderr, "input: invalid command\n");
			continue;
		}

		// 1 command case
		if(cmd_count == 1)
		{
			if(file1exist == 0 && file2exist == 0)
			{
				exec_cmd(cmd[0]);
			}
			else if(file1exist == 1)
			{
				int child_status;
				if(fork() == 0)
				{
					fd1 = open(file1, O_RDWR, 0644);
					dup2(fd1, 0);
					if(file2exist != 0)
					{
						int child2_status;
						if(fork() == 0)
						{
							if(file2exist == 1)
							{
								fd2 = open(file2, O_CREAT | O_RDWR | O_TRUNC, 0644);
							}
							else
							{
								fd2 = open(file2, O_APPEND | O_RDWR);
							}
							dup2(fd2, 1);
							exec_cmd(cmd[0]);
							close(fd2);
							exit(0);
						}
						else
							wait(&child2_status);
					}
					else
					{
						exec_cmd(cmd[0]);
					}
					close(fd1);
					exit(0);
				}
				else
					wait(&child_status);
			}
			else if(file2exist != 0)
			{
				int child_status;
				if(fork() == 0)
				{
					if(file2exist == 1)
					{
						fd2 = open(file2, O_CREAT | O_RDWR | O_TRUNC, 0644);
					}
					else
					{
						fd2 = open(file2, O_APPEND | O_RDWR);
					}
					dup2(fd2, 1);
					exec_cmd(cmd[0]);
					close(fd2);
					exit(0);
				}
				else
					wait(&child_status);
			}
		}
		else
		{
			/*
			for(int count = 0; count < cmd_count - 1; count++)
			{
				int pipefd[2], child_status;
				if(pipe(pipefd) == -1)
				{
					perror("pipe");
					exit(1);
				}
				
				int pid = fork();
				if(pid == 0)
				{
					if(count == 0)
					{
						close(pipefd[1]);
					}
					dup2(pipefd[0], 0);
					close(pipefd[0]);
					exec_cmd(cmd[count + 1]);
				}
				else
				{
					if(count == cmd_count - 1)
					{	
						close(pipefd[0]);
					}
					dup2(pipefd[1], 1);
					close(pipefd[1]);
					exec_cmd(cmd[count]);
				}
			}
			dup2(dup(STDIN_FILENO), 1);
			dup2(dup(STDOUT_FILENO), 0);
			*/
			for(int count = 0; count < cmd_count; count++)
			{
				int child_status, child2_status;
				if(count == 0)
				{
					if(file1exist == 0)
					{
                                		if(fork() == 0)
                                		{
                                        		fd1 = open("superrandomudontneedtoknowaboutfile.txt", O_RDWR | O_CREAT, 0644);
                                       			dup2(fd1, 1);
							exec_cmd(cmd[count]);
							close(fd1);
							exit(0);
						}
						else
							wait(&child_status);
					}
					else
					{
                                                if(fork() == 0)
                                                {
                                                        fd1 = open(file1, O_RDWR);
                                                        dup2(fd1, 0);
							if(fork() == 0)
							{
								fd2 = open("superrandomudontneedtoknowaboutfile.txt", O_RDWR | O_CREAT, 0644);
                                                        	dup2(fd2, 1);
								exec_cmd(cmd[count]);
								close(fd2);
								exit(0);
							}
							else
								wait(&child2_status);
							close(fd1);
                                                        exit(0);
                                                }
                                                else
                                                        wait(&child_status);
					}
				}
				else	
				{
					if(fork() == 0)
					{
						fd1 = open("superrandomudontneedtoknowaboutfile.txt", O_RDWR, 0644);
						dup2(fd1, 0);
						if(count + 1 != cmd_count)
						{
							if(fork() == 0)
							{
								fd2 = open("superrandomudontneedtoknowaboutfile2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
								dup2(fd2, 1);
								exec_cmd(cmd[count]);
								close(fd2);
								exit(0);
							}
							else
								wait(&child2_status);
							
							unlink("superrandomudontneedtoknowaboutfile2.txt");
						}
						else
						{
							if(file2exist != 0)
							{
								if(fork() == 0)
								{
									if(file2exist == 1)
									{
										fd2 = open(file2, O_CREAT | O_RDWR | O_TRUNC, 0644);
									}
									else
									{
										fd2 = open(file2, O_APPEND | O_RDWR);
									}
									dup2(fd2, 1);
									exec_cmd(cmd[0]);
									close(fd2);
									exit(0);
								}
								else
									wait(&child2_status);
							}
							else
							{
								exec_cmd(cmd[count]);	
							}	
						}
						close(fd1);
						exit(0);
					}
					else
						wait(&child_status);
				}
			}
			unlink("superrandomudontneedtoknowaboutfile.txt");
		}
	}
}

void signal_handler(int sig)
{
//	if(sig == SIGTSTP)
//	{
//		kill(pid, SIGTSTP);
//	}
	return;
}

int verify_commandline(char *cmd[50], int cmd_count)
{
	int success[cmd_count];

	for(int i = 0; i < cmd_count; i++)
	{
		success[i] = 0;
		for(int j = 0; j < 6; j++)
		{
			if(strncmp(cmd[i], executable[j], strlen(executable[j])) == 0)
			{
				success[i] = 1;
				break;
			}
		}
		
		for(int j = 0; j < 7; j++)
		{
			if(strncmp(cmd[i], my_execs[j], strlen(my_execs[j])) == 0)
			{
				success[i] = 1;
				break;
			}
		}

		for(int j = 0; j < 2; j++)
		{
			if(strncmp(cmd[i], builtin_cmd[j], strlen(builtin_cmd[j])) == 0)
			{	
				success[i] = 1;
			}
		}
		
		if(strncmp(cmd[i], path, strlen(path)) == 0)
		{
			success[i] = 1;
		}
		if(success[i] == 0)
		{
			return 0;
		}
	}

	return 1;
}

int check_type(char *cmd)
{
	for(int i = 0; i < 6; i++)
	{
		if(strncmp(cmd, executable[i], strlen(executable[i])) == 0)
		{
			return 1;
		}
	}

	for(int i = 0; i < 7; i++)
	{
		if(strncmp(cmd, my_execs[i], strlen(my_execs[i])) == 0)
		{
			return 2;
		}
	}


	for(int i = 0; i < 2; i++)
	{
		if(strncmp(cmd, builtin_cmd[i], strlen(builtin_cmd[i])) == 0)
		{
			return 3;
		}
	}

	if(strncmp(cmd, path, strlen(path)) == 0)
	{
		return 4;
	}

	return 0;
}


void exec_cmd(char *cmd)
{
	//separate arguments
	char *arg[50];
	int num = 0, child_status;
	char *ptr = strtok(cmd, " ");
	while (ptr != NULL) 
	{
		arg[num++] = ptr;
		ptr = strtok(NULL, " ");
	}
	arg[num] = NULL;
	char path[200];

	//executables
	if(check_type(arg[0]) == 1)
	{
		sprintf(path, "/bin/%s", arg[0]);

		if (fork() == 0) 
		{
			execv(path, arg);
			exit(0);
		}
		else
			wait(&child_status);
	}
	//my execs
	else if(check_type(arg[0]) == 2)
	{
		sprintf(path, "./%s", arg[0]);

		if (fork() == 0)
                {
                        execv(path, arg);
                        exit(0);
                }
                else
                        wait(&child_status);
	}
	//builtin_cmd
	else if(check_type(arg[0]) == 3)
	{
		if(strncmp(arg[0], "cd", 2) == 0)
		{
			if(chdir(arg[1]) == -1)
			{
				if(errno == EACCES || errno == EISDIR || errno == ENOENT || errno == ENOTDIR || errno == EPERM)
				{
					perror("cd");
				}
				else
				{
					fprintf(stderr, "Error occured: %d\n", errno);
				}
			}
		}
		else
		{
			fprintf(stderr, "exit\n");
			if(num > 1)
			{
				exit(atoi(arg[1]));
			}
			exit(0);
		}
	}
	//exe binaries
	else if(check_type(arg[0]) == 4)
	{

		if (fork() == 0) 
		{
			execv(arg[0], arg);
			exit(0);
		}
		else
			wait(&child_status);	
	}
}
