#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    //system ("clear");
    printf ("Gerador de índice remissivo de um texto\nIvo Lima da Silva - 201108006\n\n");

    /* --- Verify 'sw' and 'csc' programs --- */

    char
        originalCwd[1024],
        cscPath[1024],
        swPath[1024],
        cwd[1024];

    if (getcwd(originalCwd, sizeof(originalCwd)) != NULL)
        printf ("Current working dir: %s\n", originalCwd);
    else
        printf("getcwd() error.\n");

    DIR
        *pDir = opendir(originalCwd);

    struct dirent
        *dentry;

	struct stat
        stat_entry;

    unsigned short int
        flagSW = 0,
        flagCSC = 0;

    if (pDir == NULL)
	{
		printf ("Invalid directory.\n");
		return -1;
	}

	while ((dentry = readdir(pDir)) != NULL)
	{
		stat(dentry->d_name, &stat_entry);
		if (S_ISREG(stat_entry.st_mode))
		{
			if (strcmp (dentry->d_name, "sw") == 0)
                flagSW = 1;
            else if (strcmp (dentry->d_name, "csc") == 0)
                flagCSC = 1;
		}
	}

	if (!flagSW || !flagCSC)
    {
        printf ("'sw' and 'csc' programs not found in the current folder.\n");
        return -1;
    }
    else
        printf ("'sw' and 'csc' programs found.\n\n");

    /* --- Change dir --- */

    chdir (argv[1]);

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

    /* --- Verify 'n.txt' */

    int
        nFiles = 0,
        i,
        status;

    char
        file[5];

    FILE
        *pFile = fopen("words.txt", "r");

    pid_t
        pid,
        pidSon;

    for (i = 1; pFile != NULL ; i++)
    {
        sprintf (file, "%d.txt", i);
        printf ("Looking for file: %s\n", file);
        pFile = fopen(file, "r");
        if (pFile != NULL)
            nFiles++;
    }

    if (nFiles == 0)
        printf ("No files found.\n");
    else
        printf ("Number of files found: %d\n", nFiles);

    /* --- Running a SW proccess for each file --- */

    printf ("Original CWD: %s\n", originalCwd);
    strcpy (swPath, originalCwd);
    strcat (swPath, "/sw");
    printf ("SW location: %s\n", swPath);

    for (i = 0; i < nFiles; i++)
    {
        pid = fork();
        if (pid == -1)
            printf ("Error on fork().\n");
        else if (pid == 0)
        {
            dup2(0, STDOUT_FILENO);
            sprintf (file, "%d", i + 1);
            printf ("File: %s\n", file);
            execlp (swPath, "./sw", file, NULL);
        }
        else if (pid > 0)
        {
            pidSon = wait(&status);
            printf ("My son (%d) just finished.\n", pidSon);
        }
    }

    /* --- Running the CSC proccess --- */

    printf ("Original CWD: %s\n", originalCwd);
    strcpy (cscPath, originalCwd);
    strcat (cscPath, "/csc");
    printf ("SW location: %s\n", cscPath);

    pid = fork();
    if (pid == -1)
        printf ("Error on fork().\n");
    else if (pid == 0)
    {
        dup2(0, STDOUT_FILENO);
        sprintf (file, "%d", nFiles);
        execlp (cscPath, "./csc", file, NULL);
    }
    else if (pid > 0)
    {
        pidSon = wait(&status);
        printf ("My son (%d) just finished.\n", pidSon);
    }

    return 0;
}
