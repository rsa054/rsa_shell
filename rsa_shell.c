#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <errno.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_HISTORY 100

static char history[MAX_HISTORY][MAX_INPUT];
static int history_count = 0;
static int history_index = 0;

static struct termios orig_termios;

/* ---------- Terminal Mode ---------- */

void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);  // FIXED
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* ---------- History ---------- */

void add_history(const char *cmd)
{
    if (history_count < MAX_HISTORY)
    {
        strncpy(history[history_count], cmd, MAX_INPUT - 1);
        history[history_count][MAX_INPUT - 1] = '\0';
        history_count++;
    }
    history_index = history_count;
}

/* ---------- Input Handling ---------- */

void clear_line()
{
    printf("\33[2K\r"); // clear entire line
}

void read_input(char *buffer)
{
    int len = 0;
    buffer[0] = '\0';

    history_index = history_count;

    enable_raw_mode();

    char c;

    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (c == '\n')
        {
            buffer[len] = '\0';
            printf("\n");
            break;
        }

        /* BACKSPACE */
        else if (c == 127)
        {
            if (len > 0)
            {
                len--;
                buffer[len] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        }

        /* ARROW KEYS */
        else if (c == 27)
        {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
                read(STDIN_FILENO, &seq[1], 1) == 1)
            {
                if (seq[0] == '[')
                {
                    /* UP */
                    if (seq[1] == 'A')
                    {
                        if (history_index > 0)
                        {
                            history_index--;
                            clear_line();

                            strcpy(buffer, history[history_index]);
                            len = strlen(buffer);

                            printf("rsa> %s", buffer);
                            fflush(stdout);
                        }
                    }

                    /* DOWN */
                    else if (seq[1] == 'B')
                    {
                        if (history_index < history_count - 1)
                        {
                            history_index++;
                            clear_line();

                            strcpy(buffer, history[history_index]);
                            len = strlen(buffer);

                            printf("rsa> %s", buffer);
                            fflush(stdout);
                        }
                        else
                        {
                            history_index = history_count;
                            clear_line();

                            len = 0;
                            buffer[0] = '\0';

                            printf("rsa> ");
                            fflush(stdout);
                        }
                    }
                }
            }
        }

        /* NORMAL CHAR */
        else
        {
            if (len < MAX_INPUT - 1)
            {
                buffer[len++] = c;
                buffer[len] = '\0';
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }

    disable_raw_mode();
}

/* ---------- Main ---------- */

int main()
{
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    printf("Welcome to RSA Shell 🚀\n");

    while (1)
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));

        printf("rsa:%s$ ", cwd);
        fflush(stdout);

        read_input(input);

        if (strlen(input) == 0)
            continue;

        add_history(input);

        /* EXIT */
        if (strcmp(input, "exit") == 0)
        {
            printf("Goodbye 👋\n");
            break;
        }

        /* HISTORY */
        if (strcmp(input, "history") == 0)
        {
            for (int i = 0; i < history_count; i++)
                printf("%d %s\n", i, history[i]);
            continue;
        }

        /* TOKENIZE */
        int i = 0;
        char *token = strtok(input, " ");

        while (token && i < MAX_ARGS - 1)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        args[i] = NULL;

        /* CD */
        if (args[0] && strcmp(args[0], "cd") == 0)
        {
            if (!args[1])
            {
                printf("cd: missing argument\n");
            }
            else if (chdir(args[1]) != 0)
            {
                perror("cd");
            }
            continue;
        }

        /* EXECUTION */
        pid_t pid = fork();

        if (pid == 0)
        {
            execvp(args[0], args);
            perror("exec failed");
            exit(1);
        }
        else if (pid > 0)
        {
            int status;
            waitpid(pid, &status, 0);
        }
        else
        {
            perror("fork failed");
        }
    }

    return 0;
}