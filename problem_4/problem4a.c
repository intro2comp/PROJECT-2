#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <ctype.h>
#include <limits.h>

typedef enum { false, true } bool;

union sigval cs;
int currentValue;

char* createMinMaxSumString(int minimum, int maximum, int sum){
    char* str  = malloc(sizeof(char) * 100);
    sprintf(str, "%d %d %d", minimum, maximum, sum);
    return str;
}

char* createNewMinMaxSumString(char* oldString, int currVal){
    int currentMinimum= 0 , currentMaximum = 0, currentSum= 0;
    sscanf(oldString, "%d %d %d", &currentMinimum, &currentMaximum, &currentSum);
    
    if(currentValue > currentMaximum){
        currentMaximum = currentValue;
    }
    
    if(currentValue < currentMinimum){
        currentMinimum = currentValue;
    }
    
    currentSum = currentSum + currentValue;
    return createMinMaxSumString(currentMinimum, currentMaximum, currentSum);
}


void sig_action_function(int sig, siginfo_t *info, void *ptr)
{
    cs = info->si_value;
    cs.sival_ptr =  createNewMinMaxSumString(cs.sival_ptr, currentValue);
    printf("Signal: %d Sum: %s\n", info->si_pid, (char*) cs.sival_ptr);
}


typedef struct Struct{
    int* integerArray;
    int arraySize;
} Struct;

const int DEBUG = 0;

int parseFile(char* filePath){
    FILE* file = fopen(filePath, "r");
    int fd = fileno(file);
    char buffer;
    while(read(fd, &buffer, 1) != 0){
        printf("%c", buffer);
    }
    
    fclose (file);
    return 0;
}

Struct* parseIntFile(char* filePath){
    FILE* file = fopen(filePath, "r");
    int fd = fileno(file);
    char buffer;
    int count = 0;
    while(read(fd, &buffer, 1) != 0){
        if(buffer == '\n'){
            count++;
        }
    }
    
    fclose(file);
    
    int* nums = malloc(sizeof(int) * count);
    
    file = fopen(filePath, "r");
    
    int num = 0;
    int currentCount = 0;;
    while(!feof(file) && (currentCount < count)){
        fscanf (file, "%d", &num);
        nums[currentCount] = num;
        currentCount++;
    }
    
    fclose(file);
    
    Struct *ret = malloc(sizeof(Struct));
    
    ret->integerArray = nums;
    ret->arraySize = count;
    
    
    return ret;
}

int getMaximum(Struct* nums){
    int ret = INT_MIN;
    
    int i;
    for(i = 0; i < nums->arraySize; i++){
        if(nums->integerArray[i] > ret){
            ret = nums->integerArray[i];
        }
    }
    
    return ret;
}

int getMinimum(Struct* nums){
    int ret = INT_MAX;
    
    int i;
    for(i = 0; i < nums->arraySize; i++){
        if(nums->integerArray[i] < ret){
            ret = nums->integerArray[i];
        }
    }
    
    return ret;
}

int getSum(Struct* nums){
    int ret = 0;
    
    int i;
    for(i = 0; i < nums->arraySize; i++){
        ret += nums->integerArray[i];
    }
    
    return ret;
}

void printSum(char* infoString){
    int currentMinimum= 0 , currentMaximum = 0, currentSum= 0;
    sscanf(infoString, "%d %d %d", &currentMinimum, &currentMaximum, &currentSum);
    printf("Max=%d\n", currentMaximum);
    printf("Min=%d\n", currentMinimum);
    printf("Sum=%d\n", currentSum);
    return;
}

int multiProcessor(Struct* nums, int ind){
    pid_t pid;
    printf("hi, I am process %d and my parent is %d.\n", getpid(), getppid());
    int stat;
    pid = fork();
    if(pid == 0){
        currentValue = nums->integerArray[ind];
        if(ind == nums->arraySize - 1){
            if(DEBUG) { printf("%d at index %d FINAL CHILD\n", nums->integerArray[ind], ind);}
            cs.sival_ptr = createMinMaxSumString(currentValue, currentValue, currentValue);
