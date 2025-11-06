#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <assert.h>

#pragma region MACROS
//===================================================
// Macros
//===================================================

// #define DEBUG

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) (void)(0)
#endif

#define INPUT_SIZE 256
#define MAX_TOKENS 50

#define SIZE(X) sizeof(X) / sizeof(X[0])

//===================================================
#pragma endregion

typedef struct
{
    int file_descriptor;
    int flags;
} redirect;

bool check_if_redirect(const char *str, redirect *redir)
{
    bool user_specified_descriptor = false;
    if (isdigit(str[0]))
    {
        redir->file_descriptor = str[0] - '0';
        user_specified_descriptor = true;
        str++;
    }

    if (strcmp(str, ">>") == 0)
    {
        redir->flags = O_WRONLY | O_APPEND | O_CREAT;
    }
    else if (strcmp(str, ">") == 0)
    {
        redir->flags = O_WRONLY | O_CREAT | O_TRUNC;
    }
    else if (strcmp(str, "<") == 0)
    {
        redir->flags = O_RDONLY;
    }
    else
    {
        return false;
    }

    if (!user_specified_descriptor)
    {
        if (redir->flags & O_WRONLY)
            redir->file_descriptor = STDOUT_FILENO;
        else
            redir->file_descriptor = STDIN_FILENO;
    }

    return true;
}

void launch_program(char *tokens[], char *filename, redirect *redir)
{

    pid_t pid = fork();

    LOG("executing with pid = %d\n", pid);

    if (pid != 0)
        // Parent process to wait until child executes
        wait(NULL);
    else
    // Child process
    {
        if (redir->file_descriptor != -1)
        {
            int fd = open(filename, redir->flags, 0644);
            if (fd < 0)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, redir->file_descriptor);
            close(fd);
        }

        LOG("begging to execute command\n");
        execvp(tokens[0], tokens);

        perror("execvp");
        exit(EXIT_FAILURE);
    }
}

void parse_tokens(char *input, char *tokens[], char **filename, redirect *redir)
{

    int i = 0;
    tokens[i] = strtok(input, " ");

    while (tokens[i] != NULL)
    {
        if (strcmp(tokens[i], "exit") == 0 && i == 0)
        {
            printf("Exiting...\n");
            exit(0);
        }
        else if (i == 0)
        {
            LOG("Program: %s \n", tokens[i]);
        }
        else if (check_if_redirect(tokens[i], redir))
        {
            LOG("Redirection operator: %s \n", tokens[i]);
            tokens[i] = NULL;
        }
        else if (redir->file_descriptor != -1)
        {
            LOG("File: %s \n", tokens[i]);
            (*filename) = tokens[i];
        }
        else
        {
            LOG("Arg: %s \n", tokens[i]);
        }

        ++i;
        tokens[i] = strtok(NULL, " ");
    }
}

int main(int argc, char const *argv[])
{
    char input[INPUT_SIZE] = {};

    while (fgets(input, INPUT_SIZE, stdin) != NULL)
    {
        char *tokens[MAX_TOKENS] = {};
        char *filename = NULL;
        redirect redir = {.file_descriptor = -1, .flags = 0};

        //=====================================
        // Remove newline from input
        //=====================================

        char *newline = NULL;
        newline = strchr(input, '\n');
        if (newline != NULL)
            *newline = '\0';

        //=====================================
        // Split into tokens
        // Locate program, args, redirection
        // and filename
        //=====================================

        parse_tokens(input, tokens, &filename, &redir);

        //=====================================
        // Launch the command in a child process
        //=====================================

        launch_program(tokens, filename, &redir);
    }

    puts("");

    return 0;
}
