#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <stdlib.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}
int main(int c,char**v){
 if(c!=2||!isnum(v[1])) usage(v[0]);

 char path[256];
 char comm[256];
 char state;
 int pid, ppid;
 char cmd[256];
 long total_ticks, ticks_per_sec, utime, stime;
 double total_time;
 char line[256];
 
 snprintf(path, sizeof(path), "/proc/%s/stat", v[1]);
 FILE *f = fopen(path, "r");
 if(fscanf(f, "%d (%[^)]) %c %d %*d %*d %*d %*d %*d %*d %*d %*d %*d %ld %ld", &pid, comm, &state, &ppid, &utime, &stime) == 6){
    printf("PID:%d\n", pid);
    printf("State:%c\n", state);
    printf("PPID:%d\n", ppid);
 }
 fclose(f);

 snprintf(path, sizeof(path), "/proc/%s/cmdline", v[1]);
 FILE *g = fopen(path, "r");
 if(fscanf(g, "%s", cmd) == 1){
    printf("Cmd:%s\n", cmd);
 }
 fclose(g);

 total_ticks = utime + stime;
 ticks_per_sec = sysconf(_SC_CLK_TCK);
 total_time = (double)(utime + stime) / ticks_per_sec;
 printf("CPU:%ld %.3f\n",total_ticks, total_time);

 snprintf(path, sizeof(path), "/proc/%s/status", v[1]);
 FILE *h = fopen(path, "r");
 if(h){
    while(fgets(line, sizeof(line), h)){
        if(strncmp(line, "VmRSS:", 6) == 0){
            int rss;
            sscanf(line, "VmRSS: %d", &rss);
            printf("VmRSS:%d\n", rss);
        }
    }
 }
 fclose(h);

 return 0;
}
