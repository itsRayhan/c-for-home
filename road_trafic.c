#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
void waitSec(int s) { Sleep(s * 1000); }
#else
#include <unistd.h>
void waitSec(int s) { sleep(s); }
#endif

// --------- Config ---------
#define LANES 4

typedef enum
{
    RED = 0,
    YELLOW = 1,
    GREEN = 2
} Signal;

typedef struct
{
    char name[20];
    Signal signal;
    int greenTime;
    int yellowTime;
    int redTime;         // computed per cycle
    int vehiclesWaiting; // simulated input
} Road;

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

const char *signalToStr(Signal s)
{
    if (s == GREEN)
        return "GREEN ";
    if (s == YELLOW)
        return "YELLOW";
    return "RED   ";
}

void printHeader()
{
    printf("===============================================\n");
    printf("   BD ROAD TRAFFIC CONTROL SYSTEM (WUB)\n");
    printf("   Simulation of 4-way intersection signals\n");
    printf("===============================================\n\n");
}

void printRoadStatus(Road r[])
{
    printf("Intersection Status (N, S, E, W)\n");
    printf("-----------------------------------------------\n");
    printf("%-8s | %-7s | %-12s | %-10s\n", "Road", "Signal", "VehiclesWait", "GreenTime");
    printf("-----------------------------------------------\n");
    for (int i = 0; i < LANES; i++)
    {
        printf("%-8s | %-7s | %-12d | %-10d\n",
               r[i].name, signalToStr(r[i].signal), r[i].vehiclesWaiting, r[i].greenTime);
    }
    printf("-----------------------------------------------\n\n");
}

int totalGreenTime(Road r[])
{
    int sum = 0;
    for (int i = 0; i < LANES; i++)
        sum += r[i].greenTime;
    return sum;
}

int totalYellowTime(Road r[])
{
    int sum = 0;
    for (int i = 0; i < LANES; i++)
        sum += r[i].yellowTime;
    return sum;
}

void resetSignalsToRed(Road r[])
{
    for (int i = 0; i < LANES; i++)
        r[i].signal = RED;
}

void setVehicles(Road r[])
{
    printf("\nEnter vehicles waiting on each road:\n");
    for (int i = 0; i < LANES; i++)
    {
        printf("  %s vehicles: ", r[i].name);
        scanf("%d", &r[i].vehiclesWaiting);
        if (r[i].vehiclesWaiting < 0)
            r[i].vehiclesWaiting = 0;
    }
}

void applyModeTimings(Road r[], int mode)
{
    // mode 1 = Normal, mode 2 = Rush
    // You can adjust times here
    for (int i = 0; i < LANES; i++)
    {
        r[i].yellowTime = 3;
        if (mode == 1)
        {
            r[i].greenTime = 8; // normal green
        }
        else
        {
            r[i].greenTime = 12; // rush green
        }
    }
}

void adaptiveGreen(Road r[], int baseGreen, int maxExtra)
{
    // Increase green time based on vehicles waiting (simple adaptive logic)
    // extra = vehicles / 5  (cap at maxExtra)
    for (int i = 0; i < LANES; i++)
    {
        int extra = r[i].vehiclesWaiting / 5;
        if (extra > maxExtra)
            extra = maxExtra;
        r[i].greenTime = baseGreen + extra;
    }
}

void computeRedTimes(Road r[])
{
    // For each road, red time = sum of other roads' green+yellow
    for (int i = 0; i < LANES; i++)
    {
        int red = 0;
        for (int j = 0; j < LANES; j++)
        {
            if (i == j)
                continue;
            red += (r[j].greenTime + r[j].yellowTime);
        }
        r[i].redTime = red;
    }
}

void countdownPrint(const char *phaseName, int sec)
{
    for (int t = sec; t >= 1; t--)
    {
        printf("\r%s: %2d sec remaining...", phaseName, t);
        fflush(stdout);
        waitSec(1);
    }
    printf("\r%s: Done!                \n", phaseName);
}

void simulateOneRoadPhase(Road r[], int idx)
{
    // Set idx as GREEN, others RED
    resetSignalsToRed(r);
    r[idx].signal = GREEN;

    clearScreen();
    printHeader();
    printf(">> Phase: %s GREEN\n\n", r[idx].name);
    printRoadStatus(r);
    countdownPrint("GREEN", r[idx].greenTime);

    // YELLOW
    r[idx].signal = YELLOW;
    clearScreen();
    printHeader();
    printf(">> Phase: %s YELLOW\n\n", r[idx].name);
    printRoadStatus(r);
    countdownPrint("YELLOW", r[idx].yellowTime);

    // Back to RED (next road will become green)
    r[idx].signal = RED;
}

void simulateCycle(Road r[], int order[])
{
    computeRedTimes(r);

    clearScreen();
    printHeader();
    printf("Starting full signal cycle...\n\n");
    printRoadStatus(r);
    printf("Cycle Info: Total Green=%d sec, Total Yellow=%d sec\n\n",
           totalGreenTime(r), totalYellowTime(r));
    waitSec(2);

    for (int k = 0; k < LANES; k++)
    {
        simulateOneRoadPhase(r, order[k]);
    }

    clearScreen();
    printHeader();
    printf("✅ Cycle completed!\n\n");
    printRoadStatus(r);
    waitSec(2);
}

void showMenu()
{
    printf("\nMENU\n");
    printf("1) Normal Mode (Fixed timings)\n");
    printf("2) Rush Hour Mode (Longer green)\n");
    printf("3) Adaptive Mode (Green depends on vehicles)\n");
    printf("4) Manual Override (Choose first green road)\n");
    printf("5) Exit\n");
    printf("Choose: ");
}

int main()
{
    Road roads[LANES] = {
        {"North", RED, 8, 3, 0, 0},
        {"South", RED, 8, 3, 0, 0},
        {"East", RED, 8, 3, 0, 0},
        {"West", RED, 8, 3, 0, 0}};

    int choice;
    int order[LANES] = {0, 2, 1, 3}; // default order: North -> East -> South -> West

    while (1)
    {
        clearScreen();
        printHeader();
        printRoadStatus(roads);
        showMenu();

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Exiting.\n");
            break;
        }

        if (choice == 5)
        {
            printf("\nGoodbye!\n");
            break;
        }

        if (choice == 1)
        {
            // Normal
            applyModeTimings(roads, 1);
            setVehicles(roads);
            simulateCycle(roads, order);
        }
        else if (choice == 2)
        {
            // Rush
            applyModeTimings(roads, 2);
            setVehicles(roads);
            simulateCycle(roads, order);
        }
        else if (choice == 3)
        {
            // Adaptive
            setVehicles(roads);
            adaptiveGreen(roads, 8, 8); // base 8, max extra 8
            simulateCycle(roads, order);
        }
        else if (choice == 4)
        {
            // Manual override
            int first;
            printf("\nChoose first GREEN road:\n");
            printf("0=North, 1=South, 2=East, 3=West : ");
            scanf("%d", &first);

            if (first < 0 || first > 3)
                first = 0;

            // Build new order starting from "first"
            int used[LANES] = {0};
            int newOrder[LANES];
            newOrder[0] = first;
            used[first] = 1;

            int pos = 1;
            for (int i = 0; i < LANES; i++)
            {
                if (!used[i])
                    newOrder[pos++] = i;
            }

            // copy back
            for (int i = 0; i < LANES; i++)
                order[i] = newOrder[i];

            setVehicles(roads);
            applyModeTimings(roads, 1);
            simulateCycle(roads, order);
        }
        else
        {
            printf("\nInvalid choice. Try again.\n");
            waitSec(2);
        }
    }

    return 0;
}