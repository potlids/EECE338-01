#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void insertion_C(int* arr, int n);
void insertion_ASM(int* arr, int n);

int main(int argc, char *argv[])
{
    int n = atoi(argv[1]);

    int i, j, k;
    srand(time(NULL));

    int *target = (int *)malloc(n * sizeof(int));
    for (i = 0; i < n; i++){
        target[i] = i+1;
    }
    for (j = 0; j < n ; j++){
        int change = rand() % (n);
        int temp = target[j];
        target[j] = target[change];
        target[change] = temp;
    }

    int *target_C = (int *)malloc(n * sizeof(int));
    int *target_ASM = (int *)malloc(n * sizeof(int));

    for (k = 0 ; k < n ; k++){
        target_C[k] = target[k];
        target_ASM[k] = target[k];
    }

    struct timespec C_start, C_end, ASM_start, ASM_end;
    double C_time, ASM_time = 0;

    clock_gettime(CLOCK_REALTIME, &C_start);
    insertion_C(target_C, n);
    clock_gettime(CLOCK_REALTIME, &C_end);
    C_time = (C_end.tv_sec - C_start.tv_sec) + (C_end.tv_nsec - C_start.tv_nsec)/1e9;

    clock_gettime(CLOCK_REALTIME, &ASM_start);
    insertion_ASM(target_ASM, n);
    clock_gettime(CLOCK_REALTIME, &ASM_end);
    ASM_time = (ASM_end.tv_sec - ASM_start.tv_sec) + (ASM_end.tv_nsec - ASM_start.tv_nsec)/1e9;

    if(n <= 20){
        printf("Before sort      : [ ");
        for(i = 0; i < n; i++){
            printf("%d ", target[i]);
        }
        printf("]\n");
        printf("After sort   (C): [ ");
        for(i = 0; i < n; i++){
            printf("%d ", target_C[i]);
        }
        printf("]\n");
        printf("After sort (ASM): [ ");
        for(i = 0; i < n; i++){
            printf("%d ", target_ASM[i]);
        }
        printf("]\n");
    }
    printf("Execution Time   (C): %.lf[s]", C_time);
    printf("Execution Time (ASM): %.lf[s]", ASM_time);

    return 0;
}

void insertion_C(int* arr, int n){
    int i,j = 0;
    int target = 0;
    for(i = 1 ; i < n ; i++){
        target = arr[i];
        j = i;
        while((arr[j-1]>arr[j])&&(j>0)){
            arr[j] = arr[j-1];
            j--;
        }
        arr[j] = target;
    }
}

void insertion_ASM(int* arr, int n){
    asm(
        "mov r4, #1\n\t" // i = 1
        "ldr r8, [%[n]\n\t]" // n
        
        "for:\n\t" // for loop execution 
        "mov r5, r4\n\t" // j = i
        "mul r4, r4, #4\n\t" // address offset 4*i

        "while:\n\t" // while loop execution
        //condition loop
        "ldr r6, [%[arr], r4]" // arr[j]
        "sub r4, r4, #4\n\t" // -4
        "ldr r7, [%[arr], r4]\n\t" // arr[j-1]
        "add r4, r4 #4\n\t" // +4
        "cmp(r6, r7)\n\t" // arr[j] - arr[j-1]
        "bgt exit\n\t" // larger than 0, exit
        "cmp (r5, #0)\n\t" // j - 0
        "blt exit\n\t" // smaller than 0, exit

        //activation loop
        "str r7, [%[arr], r4]\n\t" // store to arr[j]
        "sub r4, r4, #4\n\t" // -4
        "sub r5, r5, #1\n\t" // j-1
        "b while\n\t"

        //exit
        "exit:\n\t" // exit 
        "str r6, [%[arr], r4]\n\t" // store to arr[j]
        "udiv r4, r4, #4\n\t" // changing to i
        "add r4, r4, #1\n\t" // i+1
        "cmp(r4, r8)\n\t" // i - n
        "blt for\n\t" // smaller than 0, for loop

        :
        //input operands
        :
        [n] "m"(n), [arr] "r"(arr)
        //output operands
        :
        "r4", "r5", "r6", "r7"
        //clobbers
    );
}
