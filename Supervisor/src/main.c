#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "marvelmind.h"
#include "comm.h"
#include "main.h"

typedef struct {
    struct MarvelmindHedge * hedge;
    struct sockaddr_in * supervisor_adr;
    int sd;
} positionReportArgs_t;

bool terminateProgram=false;

void CtrlHandler(int signum) {
    terminateProgram=true;
}

static sem_t *sem;
struct timespec ts;
void semCallback() {
	sem_post(sem);
}

int main (int argc, char *argv[]) {

    // Initialising threads
    pthread_t positionReportThread;
    positionReportArgs_t positionReportArgs;

    // Initialising Marvelmind
    const char * ttyFileName = MARVELMIND_DEFAULT_TTY;
    struct MarvelmindHedge * hedge=createMarvelmindHedge ();
    struct FusionIMUValue fusionIMU;
    if (hedge==NULL)
    {
        puts ("Error: Unable to create MarvelmindHedge");
        return -1;
    }
    hedge->ttyFileName=ttyFileName;
    hedge->verbose=true;
    hedge->anyInputPacketCallback= semCallback;
    startMarvelmindHedge (hedge);

    // Initialising comms
    int sd;
    struct sockaddr_in supervisor_adr;
    supervisor_adr.sin_family = AF_INET;
    supervisor_adr.sin_port = htons(CBTC_SUPERVISOR_PORT);
    supervisor_adr.sin_addr.s_addr = inet_addr(CBTC_SUPERVISOR_IP);

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    // Set Ctrl-C handler
    signal (SIGINT, CtrlHandler);
    signal (SIGQUIT, CtrlHandler);

    // Launch threads
    positionReportArgs.hedge = hedge;
    positionReportArgs.supervisor_adr = &supervisor_adr;
    positionReportArgs.sd = sd;
    pthread_create(&positionReportThread, NULL, positionReport, (void *) &positionReportArgs);

    pthread_join(positionReportThread, NULL);
    stopMarvelmindHedge (hedge);
    destroyMarvelmindHedge (hedge);
    return 0;
}

void * positionReport(void * positionReportArgs) {
    positionReportArgs_t * args = (positionReportArgs_t *) positionReportArgs;
    struct MarvelmindHedge * hedge = args->hedge;
    struct sockaddr_in * supervisor_adr = args->supervisor_adr;
    int sd = args->sd;

    char acc[20];

    message_t message;
    message.id = 1;
    message.code = 200;
    message.data[0] = acc;

	sem = sem_open(DATA_INPUT_SEMAPHORE, O_CREAT, 0777, 0);
    struct FusionIMUValue fusionIMU;
    while ((!terminateProgram) && (!hedge->terminationRequired)) {

        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		{
			printf("clock_gettime error");
			pthread_exit(NULL);
		}
		ts.tv_sec += 2;
		sem_timedwait(sem,&ts);

        getFusionIMUFromMarvelmindHedge(hedge, &fusionIMU);
        //printf("Position: ax=%d, ay=%d, az=%d\n", fusionIMU.ax, fusionIMU.ay, fusionIMU.az);
        sprintf(acc, "%f", sqrt(fusionIMU.ax^2 + fusionIMU.ay^2 + fusionIMU.az^2));
        send_data(sd, supervisor_adr, message);
    }
    pthread_exit(NULL);
}
