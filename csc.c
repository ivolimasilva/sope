#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


#define READ 0
#define WRITE 1

#define MAX_SIZE    512

int main(int argc, char *argv[])
{
    system ("clear");

    int
        nFiles;

    sscanf (argv[1], "%d", &nFiles);

    printf ("Concatenate, sort and clean for %d files!\n\n", nFiles);

    /* --- Current dir --- */

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf ("Current working dir: %s\n", cwd);
    else
        printf("getcwd() error.\n");

    /* --- Verify 'words.txt' */

    FILE
        *pWords = fopen("words.txt", "r");

    if (pWords == NULL)
    {
        printf ("'words.txt' not found.\n");
        return -1;
    }
    else
        printf ("'words.txt' found.\n");

    /* --- Verify files --- */

    int
        i,
        nCommand = 0;

    FILE
        *pFile;

    char
        filename[MAX_SIZE],
        command[MAX_SIZE][MAX_SIZE];

    for (i = 0; i < nFiles; i++)
    {
        sprintf (filename, "%d_temp.txt", i + 1);
        pFile = fopen(filename, "r");
        if (pFile == NULL)
        {
            printf ("'%s' not found.\n", filename);
            return -1;
        }
        else
        {
            printf ("'%s' found.\n", filename);
            strcpy (command[nCommand++], filename);
        }
    }

    for (i = 0; i < nCommand; i++)
        printf ("Command: %s\n", command[i]);

    /* ---  --- */

    int
        dFile = open ("temp.txt", O_WRONLY | O_CREAT, 0644),
        dFileSorted = open ("tempSorted.txt", O_WRONLY | O_CREAT, 0644),
        status;

    pid_t
        pid,
        pidSon;

    /* --- Cat --- */

    printf ("\nCat\n");

    for (i = 0; i < nCommand; i++)
    {
        pid = fork();
        if (pid == -1)
            printf ("Error on fork().\n");
        else if (pid == 0)
        {
            dup2(dFile, STDOUT_FILENO);
            close (dFile);
            execlp ("cat", "cat", command[i], NULL);
        }
        else if (pid > 0)
        {
            pidSon = wait(&status);
            printf ("My son (%d) just finished.\n", pidSon);
        }
    }

    close (dFile);

    /* --- Sort -V --- */

    printf ("\nSort\n");

    pid = fork();
    if (pid == -1)
        printf ("Error on fork().\n");
    else if (pid == 0)
    {
        dup2(dFileSorted, STDOUT_FILENO);
        dup2(dFile, STDIN_FILENO);
        close (dFile);
        execlp ("sort", "sort", "-V", "temp.txt", NULL);
    }
    else if (pid > 0)
    {
        pidSon = wait(&status);
        printf ("My son (%d) just finished.\n", pidSon);
    }

    close (dFile);
    close (dFileSorted);

    /* --- Clean --- */

    char
        c1,
        c2,
        word[MAX_SIZE],
        wordTemp[MAX_SIZE],
        wordNumber[MAX_SIZE],
        line[MAX_SIZE * MAX_SIZE],
        aux[MAX_SIZE];

    FILE
        *pTemp = fopen("tempSorted.txt", "r"),
        *pIndex = fopen ("index.txt", "w");

    fprintf (pIndex, "INDEX\n\n");

    for (c1 = fscanf(pWords, "%s", word); c1 != EOF; c1 = fscanf(pWords, "%s", word))
    {
        memset(line, 0, sizeof(line));
        strcpy (line, word);
        strcat (line, " : ");

        fseek(pTemp, 0, SEEK_SET);

        int
            flagFirst = 0;

        for (c2 = fscanf(pTemp, "%s : %s", wordTemp, wordNumber); c2 != EOF; c2 = fscanf(pTemp, "%s : %s", wordTemp, wordNumber))
        {
            memset(aux, 0, sizeof(aux));
            strncpy (aux, wordTemp, strlen(word));

            if (strcmp (aux, word) == 0)
            {
                if (flagFirst == 0)
                    flagFirst = 1;
                else
                    strcat (line, ", ");

                strcat (line, wordNumber);
            }
        }

        if (flagFirst)
            fprintf (pIndex, "%s\n\n", line);
    }

    fclose (pTemp);
    fclose (pWords);


    status = remove ("temp.txt");
    if (status == 0)
        printf ("\nFile 'temp.txt' removed.\n");
    else
        printf ("\nFile 'temp.txt' not removed.\n");

    status = remove ("tempSorted.txt");
    if (status == 0)
        printf ("\nFile 'tempSorted.txt' removed.\n");
    else
        printf ("\nFile 'tempSorted.txt' not removed.\n");

    return 0;
}
