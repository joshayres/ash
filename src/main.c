#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <sys/wait.h>

char **get_input(char *input)
{
    char **command = malloc(8 * sizeof(char *));
    if(command == NULL)
    {
        perror("malloc failed");
        exit(1);
    }
    char *separator = " ";//TODO: Make this parser better so you can use quotes for files with spaces
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while(parsed != NULL)
    {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

//prebuilt functions
//TODO: Move to header file
int ash_cd(char **args);
int ash_help(char **args);
int ash_exit(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
};

int (*builtin_func[])(char **)={
    &ash_cd,
    &ash_help,
    &ash_exit,
};

int ash_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

int ash_cd(char **args)
{
    if(args[1] == NULL)
    {
        fprintf(stderr, "ash: expected argument to \"cd\"\n");
    }
    else
    {
        if(chdir(args[1]) != 0)
        {
            perror("ash");
        }
    }
    return 1;
}

int ash_help(char **args)
{
    printf("Josh Ayres's ash(Ayres Shell)\n");
    printf("It's a shell dumby come on now\n");
    printf("Then following functions are built in:\n");

    for(int i = 0; i < ash_num_builtins(); i++)
    {
        printf("%s\n", builtin_str[i]);
    }
    return 1;
}

int ash_exit(char **args)
{
    return 0;
}

int ash_launch(char **args)
{
    pid_t child_pid;
    int stat_loc;
    child_pid = fork();
    if(child_pid < 0)
    {
        perror("Fork failed");
        exit(1);
    }
    if(child_pid == 0) //This means you are the child
    {
       if(execvp(args[0], args) < 0)
       {
           perror(args[0]);
           exit(1);
        }
    }
    else
    {
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
}

int ash_execute(char **args)
{
    if(args[0] == NULL)
    {
        //They didn't put anything in
        return 1;
    }

    for(int i = 0; i < ash_num_builtins(); i++)
    {
        if(strcmp(args[0], builtin_str[i]) == 0)
        {
            return(*builtin_func[i])(args);
        }
    }

    return ash_launch(args);
}

#define ASH_RL_BUFSIZE 1024
char *ash_readline(void)
{
   int bufsize = ASH_RL_BUFSIZE;
   int position = 0;
   char *buffer = malloc(sizeof(char) * bufsize);
   int c;

   if(!buffer)
   {
       fprintf(stderr, "ash: alloc error\n");
       exit(EXIT_FAILURE);
   }

   while(1)
   {
       c = getchar();
       if(c == EOF || c == '\n')
       {
           buffer[position] = '\0';
           return buffer;
       }
       //TODO: Put in escape codes and shit here
       //ie \t for tab autocomplete ^L for clear [A for up
       else
       {
           buffer[position] = c;
       }
       position++;

       //If we exceed the buffer allocation
       if(position >= bufsize)
       {
           bufsize += ASH_RL_BUFSIZE;
           buffer = realloc(buffer, bufsize);
           if(!buffer)
           {
               fprintf(stderr, "ash: alloc error\n");
               exit(EXIT_FAILURE);
           }
       }
   }
}

void ash_loop(void)
{
    char **args;
    char *input;

    char cwd[1024];
    char *username = getenv("USER");
    while(1)
    {
        getcwd(cwd, sizeof(cwd));
        printf("\033[1;92m%s %s\033[0m >", username, cwd);
        input = ash_readline();
        args = get_input(input);

        ash_execute(args);

        free(input);
        free(args);
    }
}

int main(int argc, char **argv)
{
    ash_loop();
    return 0;
}
