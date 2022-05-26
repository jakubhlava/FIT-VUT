#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

int main(int argc, char** argv) {
    int immig, maxTGen, maxTJudge, maxTPick, maxTDecide; // immig=PI maxTGen=IG maxTJudge=JG maxTPick=IT maxTDecide=JT
    pid_t pid;
    pid_t waitPid;
    int immigrantID;
    struct timespec t;
    int us;
    sem_t * noJudge = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    sem_init(noJudge,1,1);
    sem_t * checkIn = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    sem_init(checkIn,1,1);
    sem_t * confirmation = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    sem_init(confirmation,1,0);
    sem_t * allCheckedIn = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    sem_init(allCheckedIn,1,0);
    sem_t * certificate = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    sem_init(certificate,1,1);
    sem_t * fileWrite = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    sem_init(fileWrite,1,1);
    int * entered = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    *entered = 0;
    int * checked = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    *checked = 0;
    int * inside = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    *inside = 0;
    int * actionCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    *actionCounter = 0; 
    int * sharedCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    *sharedCounter = 0;
    int * decidedCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    *decidedCounter = 0;
    int * judge = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    *judge = 0;
    FILE ** outFile = mmap(NULL, sizeof(FILE*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    *outFile = fopen("proj2.out","w");
    int error = 0;
    setbuf(*outFile, NULL);
    if (argc != 6) {
        fputs("You've entered too few parameters.\n",stderr);
        fputs("Usage: ./proj2 <PI> <IG> <JG> <IT> <JT>\n",stderr);
        error = 1;
    } else {
        char * last = NULL;
        immig = strtol(argv[1],&last,10);
        if (*last != 0) {
            fputs("Bad argument PI, must be valid whole number!\n",stderr);
            error = 1;
        }
        maxTGen = strtol(argv[2],&last,10);
        if (*last != 0) {
            fputs("Bad argument IG, must be valid whole number!\n",stderr);
            error = 1;
        }
        maxTJudge = strtol(argv[3],&last,10);
        if (*last != 0) {
            fputs("Bad argument JG, must be valid whole number!\n",stderr);
            error = 1;
        }
        maxTPick = strtol(argv[4],&last,10);
        if (*last != 0) {
            fputs("Bad argument IT, must be valid whole number!\n",stderr);
            error = 1;
        }
        maxTDecide = strtol(argv[5],&last,10);
        if (*last != 0) {
            fputs("Bad argument JT, must be a valid whole number!\n",stderr);
            error = 1;
        }
        if (immig < 1){
            fputs("PI out of range!\n",stderr);
            error = 1;        
        }
        if (maxTGen < 0 || maxTGen > 2000) {
            fputs("IG out of range!\n",stderr);
            error = 1;  
        }
        if (maxTJudge < 0 || maxTJudge > 2000) {
            fputs("IG out of range!\n",stderr);
            error = 1;  
        }
        if (maxTPick < 0 || maxTPick > 2000) {
            fputs("IG out of range!\n",stderr);
            error = 1;  
        }
        if (maxTDecide < 0 || maxTDecide > 2000) {
            fputs("IG out of range!\n",stderr);
            error = 1;  
        } 
    }
    if (error == 1) {
        sem_destroy(noJudge);
        sem_destroy(checkIn);
        sem_destroy(confirmation);
        sem_destroy(allCheckedIn);
        sem_destroy(certificate);
        fclose(*outFile);
        exit(1);
    }
    pid = fork();
    if (pid == 0) {
        for (int i = 0; i < immig; i++) {
            pid_t pidI = fork();
            if (pidI == 0) {
                immigrantID = ++(*sharedCounter); // prictu k pocitadlu imigrantu
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: IMM %i\t\t: starts\n",++(*actionCounter), immigrantID); // proces nastartoval
                sem_post(fileWrite);
                sem_wait(noJudge); // pocka, az se odemce budova
                (*entered)++;
                (*inside)++; // zapocita se do poctu pritomnych a vejde
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: IMM %i\t\t: enters \t\t: %i\t: %i\t: %i\n",++(*actionCounter), immigrantID, *entered, *checked, *inside);
                sem_post(fileWrite);
                sem_post(noJudge); // odemce za sebou turniket
                sem_wait(checkIn); // zapise se
                (*checked)++;
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: IMM %i\t\t: checks \t\t: %i\t: %i\t: %i\n",++(*actionCounter), immigrantID, *entered, *checked, *inside);
                sem_post(fileWrite);
                if (*judge == 1 && *entered == *checked) { // overujeme, zda se zapsali vsichni pritomni, aby soudce mohl zacit rozhodovat
                    sem_post(allCheckedIn);
                } else {
                    sem_post(checkIn);
                }
                sem_wait(confirmation); // ceka na potrvzeni
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: IMM %i\t\t: wants certificate \t: %i\t: %i\t: %i\n",++(*actionCounter), immigrantID, *entered, *checked, *inside);
                sem_post(fileWrite);                
                sem_wait(certificate);  // ceka na okenko
                if (maxTPick != 0) {
                    srand(time(0)); 
                    us = rand()%(maxTPick*1000);
                    t.tv_sec = us/1000000; // prevod na sekundy
                    t.tv_nsec = (us-(t.tv_sec*1000000))*1000; // zbytek v nanosekundach
                    nanosleep(&t, NULL); // uspi se na nahodnou dobu IT
                }
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: IMM %i\t\t: got certificate \t: %i\t: %i\t: %i\n",++(*actionCounter), immigrantID, *entered, *checked, *inside);
                sem_post(fileWrite);
                sem_post(certificate); // pusti dalsiho k okenku
                sem_wait(noJudge); // pocka az soudce odejde
                (*inside)--;
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: IMM %i\t\t: leaves \t\t: %i\t: %i\t: %i\n",++(*actionCounter), immigrantID, *entered, *checked, *inside); // odchazi
                sem_post(fileWrite);
                sem_post(noJudge); // otvira za sebou turniket
                exit(0);
                break;
            } 
            if (maxTGen != 0) {
                srand(time(0)); 
                us = rand()%(maxTGen*1000);
                t.tv_sec = us/1000000; // prevod na sekundy
                t.tv_nsec = (us-(t.tv_sec*1000000))*1000; // zbytek v nanosekundach
                nanosleep(&t, NULL); // uspi se na nahodnou dobu IG
            }
        }
    } else {
        pid_t pid2 = fork();
        if (pid2 == 0) {           
            while(*decidedCounter < immig) {  // dokud soudce ma okom rozhodovat
                if (maxTJudge != 0) {
                    srand(time(0)); 
                    us = rand()%(maxTJudge*1000);
                    t.tv_sec = us/1000000; // prevod na sekundy
                    t.tv_nsec = (us-(t.tv_sec*1000000))*1000; // zbytek v nanosekundach
                    nanosleep(&t, NULL); // uspi se na nahodnou dobu JG
                }
                sem_wait(fileWrite); 
                fprintf(*outFile, "%i\t: JUDGE\t\t: wants to enter\n",++(*actionCounter));
                sem_post(fileWrite);
                sem_wait(noJudge); // zamkne semafor noJudge = je v budove
                sem_wait(checkIn); // pocka u turniketu
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: JUDGE\t\t: enters \t\t: %i\t: %i\t: %i\n",++(*actionCounter), *entered, *checked, *inside); 
                sem_post(fileWrite);
                *judge = 1; // vejde
                if(*entered > *checked) { // pocka nez se checknou immigranti, kteri se nechecknuli
                    ++(*actionCounter);
                    sem_wait(fileWrite); 
                    fprintf(*outFile, "%i\t: JUDGE\t\t: waits for imm \t: %i\t: %i\t: %i\n",++(*actionCounter), *entered, *checked, *inside);
                    sem_post(fileWrite);
                    sem_post(checkIn);
                    sem_wait(allCheckedIn);
                }
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: JUDGE\t\t: starts confirmation \t: %i\t: %i\t: %i\n",++(*actionCounter), *entered, *checked, *inside); // zacne potvrzovani
                sem_post(fileWrite);
                if (maxTDecide != 0) {
                    srand(time(0)); 
                    us = rand()%(maxTDecide*1000);
                    t.tv_sec = us/1000000; // prevod na sekundy
                    t.tv_nsec = (us-(t.tv_sec*1000000))*1000; // zbytek v nanosekundach
                    nanosleep(&t, NULL); // uspi se na dobu JT
                }
                sem_wait(fileWrite);
                fprintf(*outFile, "%i\t: JUDGE\t\t: ends confirmation \t: %i\t: %i\t: %i\n",++(*actionCounter), *entered, *checked, *inside); // ukonci
                sem_post(fileWrite);
                for (int i = 0; i < *checked; i++) { // odemce tolikrat semafor, kolik potvrzeni vydal
                    sem_post(confirmation);
                }
                *decidedCounter = *decidedCounter + *checked; // pricteme si k poctu rozhodnutych pripadu, at muzeme korektne porovnavat s celkovym poctem
                *entered = 0;
                *checked = 0;
                if (maxTDecide != 0) {
                    srand(time(0)); 
                    us = rand()%(maxTDecide*1000);
                    t.tv_sec = us/1000000; // prevod na sekundy
                    t.tv_nsec = (us-(t.tv_sec*1000000))*1000; // zbytek v nanosekundach
                    nanosleep(&t, NULL); // uspi se na dobu JT
                }
                *judge = 0;
                sem_post(checkIn); // odemce checkin
                sem_wait(fileWrite); 
                fprintf(*outFile, "%i\t: JUDGE\t\t: leaves \t\t: %i\t: %i\t: %i\n",++(*actionCounter), *entered, *checked, *inside); // odchazi
                sem_post(fileWrite);
                sem_post(noJudge); // odemce vchod do budovy
            }
            sem_wait(fileWrite);
            fprintf(*outFile, "%i\t: JUDGE\t\t: finishes\n",++(*actionCounter)); // az nema co delat, tak se ukonci
            sem_post(fileWrite);
            exit(0);           
        }
    }
    while ((waitPid = wait(NULL)) > 0);
    sem_destroy(noJudge);
    sem_destroy(checkIn);
    sem_destroy(confirmation);
    sem_destroy(allCheckedIn);
    sem_destroy(certificate);
    fclose(*outFile);
    exit(0);
}