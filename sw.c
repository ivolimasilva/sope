#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_SIZE    512

struct stat st = {0};

char* lineParser (char *line, char *chapter)
{
    int
        n = strlen(line),
        i,
        nWord = 0,
        nNumber = 0;

    //printf ("Line Parser (%d)\n", n);

    unsigned short int
        flag = 0;

    char
        word[MAX_SIZE],
        number[MAX_SIZE],
        *newline;

    for (i = 0; i < n; i++)
    {
        if (line[i] == ':')
        {
            number[nNumber++] = '\0';
            flag = 1;
        }
        else
        {
            if (flag)
                word[nWord++] = line[i];
            else
                number[nNumber++] = line[i];
        }
    }

    word[nWord++] = '\0';

    char
        syntax[4 + strlen(chapter)];

    syntax[0] = ' ';
    syntax[1] = ':';
    syntax[2] = ' ';

    for (i = 0; i < strlen(chapter); i++)
        syntax[3 + i] = chapter[i];

    syntax[3 + i] = '-';

    newline = (char*) malloc ((nWord + nNumber + 4 + strlen(chapter)) * sizeof(char));
    strcpy (newline, word);
    strcat (newline, syntax);
    strcat (newline, number);

    //printf ("Newline: %s\n", newline);

    return newline;
}

int fileParser (char *chapter)
{
    printf ("\nFile Parser (%s)\n", chapter);

    /* --- Current directory --- */

    char cwd[MAX_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf ("Current working dir: %s\n", cwd);
    else
        printf("getcwd() error.\n");

    char
        *new_dir = cwd;

    /* --- Open temp.txt --- */

    FILE
        *pFile = fopen("temp.txt", "r");

    if (pFile == NULL)
    {
        printf ("'temp.txt' not found.\n\n");
        return -1;
    }
    else
        printf ("'temp.txt' found.\n\n");

    /* --- Create new directory --- */

    strcat (new_dir, "/temp_files");

    printf ("Looking for new dir: %s\n", new_dir);

    /*if (stat(new_dir, &st) == -1)
        mkdir(new_dir, 0700);*/

    chdir(new_dir);

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf ("Changed working dir to: %s\n", cwd);
    else
        printf("getcwd() error.\n");

    /* --- Create new temp file (organized) --- */

    char
        aux[MAX_SIZE],
        *filename;

    int
        i;

    for (i = 0; chapter[i] != '.'; i++)
        aux[i] = chapter[i];

    strcat (aux, "_\0");

    filename = (char*) malloc ((9 + strlen(chapter)) * sizeof(char));
    strcpy (filename, aux);
    strcat (filename, "temp.txt");

    printf ("\nFilename: %s\n", filename);

    FILE
        *pTempFile = fopen(filename, "w");

    if (pTempFile == NULL)
    {
        printf ("'%s' not created.\n", filename);
        return -1;
    }
    else
        printf ("'%s' created.\n", filename);

    /* --- Organizing old temp file --- */

    char
        line[MAX_SIZE],
        *newline,
        c;

    for (c = fscanf(pFile, "%s", line); c != EOF; c = fscanf(pFile, "%s", line))
    {
        //printf ("Line: %s\n", line);
        newline = lineParser (line, chapter);
        //printf ("NewLine: %s\n", newline);
        fputs (newline, pTempFile);
        fputs ("\n", pTempFile);
    }

    fclose (pTempFile);
    fclose (pFile);

    return 0;

}

int main(int argc, char *argv[])
{
    system ("clear");
    printf ("Search words for chapter %c!\n\n", argv[1][0]);

    /* --- Current dir --- */

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf ("Current working dir: %s\n", cwd);
    else
        printf("getcwd() error.\n");

    /* --- Words --- */

    FILE
        *pWords = fopen("words.txt", "r");

    if (pWords == NULL)
    {
        printf ("'words.txt' not found.\n");
        return -1;
    }
    else
        printf ("'words.txt' found.\n");

    /* --- File N --- */

    char
        filename[MAX_SIZE],
        chapter[strlen(argv[1])];

    strcpy (chapter, argv[1]);

    strcpy (filename, argv[1]);
    strcat (filename, ".txt");

    FILE
        *pFile = fopen(filename, "r");

    if (pFile == NULL)
    {
        printf ("'%s' not found.\n", filename);
        return -1;
    }
    else
        printf ("'%s' found.\n", filename);

    /* --- Pipes/Forks --- */

    int
        dFile = open ("temp.txt", O_WRONLY | O_CREAT, 0644),
        status;
    char
        c,
        word[MAX_SIZE];
    pid_t
        pid,
        pidSon;

    if (dFile == -1)
    {
        printf ("'temp.txt' not created.\n\n");
        return -1;
    }
    else
        printf ("'temp.txt' created.\n\n");

    for (c = fscanf(pWords, "%s", word); c != EOF; c = fscanf(pWords, "%s", word))
    {
        pid = fork();
        if (pid == -1)
            printf ("Error on fork().\n");
        else if (pid == 0)
        {
            dup2(dFile, STDOUT_FILENO);
            execlp ("grep", "grep", "-o", "-w", "-n", word, filename, NULL);
        }
        else if (pid > 0)
        {
            pidSon = wait(&status);
            printf ("My son (%d) just finished the word '%s'.\n", pidSon, word);
        }
    }

    /* --- Closing files --- */

    fclose (pFile);
    fclose (pWords);
    close (dFile);

    /* --- Parsing file --- */

    fileParser(chapter);

    /* --- Change dir to original and remove temp file --- */

    chdir(cwd);

    status = remove ("temp.txt");

    if (status == 0)
        printf ("\nFile 'temp.txt' removed.\n");
    else
        printf ("\nFile 'temp.txt' not removed.\n");

    return 0;
}
