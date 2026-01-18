#include <stdio.h>
//Write a C program that uses a for loop to print the first 10 natural numbers and also calculate their sum.
int main() {
    int sum=0;
    for(int i=1;i<=10;i++)
    {
        printf("%d\n",i);
        sum+=i;
    }
    printf("rayhancode %d\n", sum);
    return 0;
}