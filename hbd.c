#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <time.h>
#include "hbd.h"
#define to "to"
#define on "on"
#define clear "clear"
#define start "start"

//I really have no idea what's going on or why it won't work
//But I think I might just have to go ahead and use sendmail
//since it works on freebsd



int send(char *msg)
{
    int retval = -1;

    char cmd[300];

    char *email = "phdavis1027@gmail.com";
    char *subj = "<HBD UPDATE>";

    sprintf(cmd, "echo '%s' | mail -s '%s' %s", msg, subj, email);

    system(cmd);

    retval = 0;

    printf("Successfully sent your message!\n");

    return retval;
}

int main(int argc, char **argv) //main is only ever called to parse a command line call
{
    parse(argv); 
    return (0);    
}

int parse(char **args)
{

    int retval = -1;

    int w = 0; // indicates whether a record is being added or not,
               // which determines what kind of input is necessary for a proper command



    char *rec = NULL;
    char *date = NULL;

    char inrec[200];
    char indate[200];

    while(*args){

        if(strcmp(*args, start)==0){
            char *listening = checkconfig("listening");
            printf("HOLY MACKEREL\n");
            if(strcmp(listening, "yes") == 0){
                printf("HOLY MACKEREL\n");
                printf("hbd is already listening.\n");
            }else if(strcmp(listening, "no") == 0){
                hbdlisten();
                char *notice_time = checkconfig("notice_time");
                printf("hbd will now listen for birthdays and notify you at %s\n", notice_time);
            }
        }

        if(strcmp(*args, clear)==0){
            printf("You are attempting to permanently delete your birthday file. Are you sure?\n");
            printf("[Y] for yes: ");
            char ans[1]; 
            scanf("%c",ans);

            if (tolower(ans[0]) == 'y')
                clearbdays();
            exit(0); // if you try to clear, that can only be the last thing you try
        }

        if(strcmp(*args,to) == 0){
            w = 1;
            rec = *(++args);
            strcpy(inrec,rec);
        }

        if(strcmp(*args, on) == 0){
            w = 1;
            date = *(++args);
            validate(date);
            strcpy(indate, date);
        }

        ++args;
    }

    if(w){
        if(!rec){
            printf("Whose birthday is it? ");
            scanf("%s", inrec);
        }

        if(!date){
            printf("When is the birthday? ");
            scanf("%256s", indate);
            if(validate(indate) != 0){
                return retval;
            }
        }

        writebday(inrec, indate);
    }

    retval = 0;

    return retval;
}

void writebday(const char *who, const char *when)
{

    FILE *bdays = fopen("./storage/bdays.txt","a");
    fprintf(bdays,"%s:%s\n",who,when);
    fclose(bdays);

}

void clearbdays()
{
    FILE *bdays = fopen("./storage/bdays.txt","w");
    fclose(bdays);
}

int validate(char *date)
{ // can't be const because of strtok

    regex_t date_re;
    regmatch_t p_match[1];

    char pattern[] =  "((1)|(2)|(3)|(4)|(5)|(6)|(7)|(8)|(9)|(10)|(11)|(12))/(([0-2][0-9])|(3[01]))";
    //this is gnarly but since it won't really be put under stressful circumstances the capturing groups
    //shouldn't affect performance

    int exception = 0;
    
    char delim = '/';
    char *ptr = &delim; 
    char date_cpy[10];
    strcpy(date_cpy,date);
    char *month = strtok(date_cpy, ptr);
    char *day = strtok(NULL, ptr);

    if(atoi(month) == 2 && atoi(day) > 28){
        fprintf(stderr,"There are only 28 days in February.");
        exit(1);
    }

    if((atoi(month) == 4 ||
        atoi(month) == 5||
        atoi(month) == 9 ||
        atoi(month) == 11) && atoi(day) > 30){
        fprintf(stderr, "Month %s only has 30 days.\n", month);
        exit(1);
    }

    printf("%s\n",month);

    if(regcomp(&date_re, pattern, REG_EXTENDED) != 0){
        printf("Oh no!\n");
    }

    int match = regexec(&date_re, date, 1, p_match, 0);

    if(match != 0){
        fprintf(stderr, "Invalid birth date.\n");
        exit(1);
    }

    return 0;
}

void hbdlisten()
{

    time_t sys_time;

    time(&sys_time);

    struct tm *time;

    time = localtime(&sys_time);
}

char *checkconfig(const char *param)
{
    size_t ret_size = 256;
    char *resp = (char *) calloc(sizeof(char), ret_size);
    char *line = NULL;
    ssize_t keep_reading;
    size_t len = 0;


    FILE *config = fopen("./storage/config.txt","r");
    if(config == NULL){
        fprintf(stderr,"Can't find config file. Please add one in this program's storage directory.\n");
        exit(1);
    }

    while((keep_reading = getline(&line, &len, config)) != -1){

        char *line_copy; //shouldn't need buffering since users oughtn't interface with the config file directly
        strcpy(line_copy, line);

        const char delim = ':';
        const char *ptr = &delim;
        char *current = strtok(line_copy, ptr);
        if(strcmp(current, param) == 0){
            char *value = strtok(NULL,ptr);
            strncpy(resp, value, ret_size);
            fclose(config);
            if(line)
                free(line);
            return resp;
        }

    }

    char *something = "listening";

    if(resp[0] == 0 && strcmp(param, something) != 0){
        printf("Could not find config entry for %s\n", param);
        printf("Please enter one now.");
        char *def = (char *) calloc(sizeof(char), ret_size);
        scanf("%256s",def);
        return def;
    }

    //fclose(config);
    //if(line)
     //   free(line);

     return NULL;
}