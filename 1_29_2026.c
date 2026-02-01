#include <stdio.h>

int main() {
    int arry[]= {12,32,43,84,1,54,6,15,87,64};
    int n;

    //normal order print
    printf("normal order:");
    for (n = 0; n <= 9; n++)
    {
        printf(" %d  ", arry[n]);
    }

    // revrse order
    printf("reverse order:");
    for (n = 9; n >= 0; n--)
    {
        printf(" %d  ", arry[n]);
    }

        return 0;
}