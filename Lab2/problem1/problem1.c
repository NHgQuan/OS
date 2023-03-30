#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

const int NUMMOVIES = 1682;

int *totalScoreOfMovies;
int *numRatingOfMovies;
double *averageScoreOfMovies;

void setZeroValForGloArr(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = 0;
    }
}

void prehandleDataArray()
{
    setZeroValForGloArr(totalScoreOfMovies, NUMMOVIES + 1);
    setZeroValForGloArr(numRatingOfMovies, NUMMOVIES + 1);
}

int lenStr(const char *str)
{
    int length = 0;
    while (*(str++) != '\0')
        length++;
    return length;
}

void removeCharStr(char **str, int index)
{
    if (index > lenStr(*str) || index < 0)
        return;
    char *resultStr = (char *)malloc(lenStr(*str));
    int i = 0;
    for (; i < index; i++)
    {
        resultStr[i] = (*str)[i];
    }
    for (; i < lenStr(*str); i++)
    {
        resultStr[i] = (*str)[i + 1];
    }
    //free(*str);
    *str = resultStr;
}

char *deepCopyStr(char *str)
{
    char *nStr = (char *)malloc(lenStr(str) + 1);
    for (int i = 0; i <= lenStr(str); i++)
    {
        nStr[i] = str[i];
    }
    return nStr;
}

char *takeFirstWord(char *str, const char chr)
{
    // return null if str is empty
    if (str == NULL)
        return NULL;

    // make clone of str and standardize it
    char *cloneStr = deepCopyStr(str);
    // standardizeIntt(&cloneStr);

    // find length and create first word string
    int nStrLen = 0;
    while (cloneStr[nStrLen] != chr && cloneStr[nStrLen] != 0)
    {
        nStrLen++;
    }

    char *nStr = (char *)malloc(sizeof(char) * (nStrLen + 1));

    for (int i = 0; i < nStrLen; i++)
    {
        nStr[i] = cloneStr[i];
    }
    nStr[nStrLen] = 0;

    //free(cloneStr);

    return nStr;
}

void removeFirstWord(char **str, const char chr)
{
    char *firstWord = takeFirstWord(*str, chr);
    int lengthFirstWord = lenStr(firstWord);
    //free(firstWord);
    for (int i = 0; i < lengthFirstWord; i++)
    {
        removeCharStr(str, 0);
    }
    if (lenStr(*str))
        removeCharStr(str, 0);
    // standardizeIntt(str);
}

char *separateFirstWord(char **str, const char chr)
{
    if (*str == NULL)
        return NULL;
    // string pass have must be standardized
    char *nStr = takeFirstWord(*str, chr);

    removeFirstWord(str, chr);

    return nStr;
}

void putScoreToTotalScoreArray(char *data)
{

    removeFirstWord(&data, '\t');
    char *movieIdStr = separateFirstWord(&data, '\t');
    char *scoreStr = separateFirstWord(&data, '\t');
    int movieId = atoi(movieIdStr), score = atoi(scoreStr);
    totalScoreOfMovies[movieId] += score;
    numRatingOfMovies[movieId]++;
}

void readFileAndHandle(const char *path)
{

    FILE *file = fopen(path, "r");

    if (file == NULL)
        return ;

    char* line = (char *)malloc(100);

    while (fgets(line, 100, file) != NULL)
    {
        putScoreToTotalScoreArray(line);
    }

    //free(line);

    fclose(file);
}

void calAverageScoreArray()
{
    for (int i = 0; i < NUMMOVIES + 1; i++)
    {
        averageScoreOfMovies[i] = totalScoreOfMovies[i]*1.0 / numRatingOfMovies[i];
    }
}

int *creAndAttachSHM(size_t size, int* shmid)
{
    *shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    if (*shmid < 0)
    {
        perror("shmget");
        exit(1);
    }
    int *shm = shmat(*shmid, NULL, 0);
    if (shm == (int *)-1)
    {
        perror("shmat");
        exit(1);
    }
    return shm;
}

void removeSHM(int shmid)
{
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    { // remove the shared memory segment
        perror("shmctl");
        exit(1);
    }
}

void writeResultToFile(double* arr, int size)
{
    FILE *f = fopen("./movie-100k-split/movie-100k-result.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    for (int i = 1; i < size; i++)
    {
        fprintf(f, "%d %.1f\n",i, arr[i]);
    }

    fclose(f);
    return;
}

int main(int argc, char *argv[])
{
    averageScoreOfMovies = malloc((NUMMOVIES + 1) * sizeof(double));
    int* shmid=(int *)malloc(sizeof(int));
    int *shm = creAndAttachSHM(sizeof(int) * (NUMMOVIES + 1) * 2, shmid);
    totalScoreOfMovies = shm;
    numRatingOfMovies = shm + NUMMOVIES + 1;
    prehandleDataArray();
    int pid = fork();
    if (pid == 0)
    {
        readFileAndHandle("./movie-100k-split/movie-100k_1.txt");
           exit(0);
    }
    else
    {
        wait(NULL);
    }

    if (pid == 0)
    {
        readFileAndHandle("./movie-100k-split/movie-100k_2.txt");
           exit(0);
    }
    else
    {
        wait(NULL);
    }
    calAverageScoreArray();
    writeResultToFile(averageScoreOfMovies, NUMMOVIES + 1);
    removeSHM(*shmid);
    //free(shmid);
}