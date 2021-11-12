#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <time.h>
#include "hbd.h"
#define CONFIG_PATH "/Users/phillipdavis/Desktop/c-programs/hbd/storage/config.txt"
#define to "to"
#define on "on"
#define clear "clear"
#define start "start"
#define stop "stop"
#define _check "check"




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
            printf("I'm here\n");
            void *listening = checkconfig("listening");
            if((char*)listening == NULL){
                startlistening();
                writeconfig("listening", "yes");
                char *notice_time = checkconfig("notice_time");
                printf("hbd will now listen for birthdays and notify you at %s\n", notice_time);
            }



            if(strcmp((char*)listening, "yes") == 0){
                printf("hbd is already listening.\n");
            }else if(strcmp((char*)listening, "no") == 0){
                char *notice_time = checkconfig("notice_time");
                startlistening();
                editconfig("listening","yes");
                printf("hbd will now listen for birthdays and notify you at %s\n", notice_time);
            }

        }

        if(strcmp(*args, stop)==0){
            void *listening = checkconfig("listening");
            
            if (listening == NULL){
                writeconfig("listening","no");
                printf("hbd will not alert you for birthdays\n");
            }


            if(strcmp(listening,"no") == 0){
                printf("hbd not listening\n");
            }

            if(strcmp(listening,"yes")==0){
                stoplistening();
                editconfig(listening, "no");
                printf("hbd will not alert you for birthdays\n");
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

    FILE *bdays = fopen(CONFIG_PATH,"a");
    fprintf(bdays,"%s:%s\n",who,when);
    fclose(bdays);

}

void clearbdays()
{
    FILE *bdays = fopen(CONFIG_PATH,"w");
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

void check()
{
    printf("checking\n");
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


    FILE *config = fopen(CONFIG_PATH,"r");
    if(config == NULL){
        fprintf(stderr,"Can't find config file. Please add one in this program's storage directory.\n");
        exit(1);
    }

    while((keep_reading = getline(&line, &len, config)) != -1){


        char line_copy[1024]; //shouldn't need buffering since users oughtn't interface with the config file directly
        strcpy(line_copy, line);


        const char delim[2] = ":";
        char *current = strtok(line_copy, delim);
        if(strcmp(current, param) == 0){
            char *value = strtok(NULL,delim);
            strcpy(resp, value);
            fclose(config);
            free(line);
            stripnewline(resp);
            return resp;
        }

    }


    if(strcmp(param,"notice_time")==0){
        printf("Could not find config entry for %s\n", param);
        printf("Please enter one now. ");
        char *def = (char *) calloc(sizeof(char), ret_size);
        scanf("%256s",def);
        writeconfig(param,def);
        return def;
    }

    fclose(config);
    if(line)
       free(line);

    return NULL;
}

int writeconfig(const char * key, const char * val)
{
    FILE *config = fopen(CONFIG_PATH,"a");
    fprintf(config, "%s:%s\n",key,val);
    fclose(config);
    return 0;
}

int editconfig(const char * key, const char * val)
{

    FILE *old_config = fopen("./storage/config.txt","r");
    FILE *new_config = fopen("./storage/newconfig.txt","w");
    
    int keep_reading;
    char *line = NULL;
    size_t len = 0;

    while((keep_reading = getline(&line, &len, old_config)) != -1){
        char line_copy[100];        
        strcpy(line_copy, line);

        char delim = ':';
        char *ptr = &delim;
        char *current_entry = strtok(line_copy, ptr);


        if(strcmp(current_entry, key) == 0){
            fprintf(new_config,"%s:%s\n", key, val);
        }else{
            fprintf(new_config,"%s:%s\n",line,strtok(NULL,ptr));
        }
    }

    fclose(old_config);
    remove("./storage/config.txt");

    fclose(new_config);
    rename("./storage/newconfig.txt","./storage/config.txt");

    return 0;
}


void stripnewline(char *input)
{
    input[strcspn(input,"\n")] = 0;
}

void startlistening()
{
    char * magic_word = "hbd check";
    char * cmdformat = "(crontab -l ; echo '%s * * * * %s') | crontab -";
    char cmd[200]; 
    sprintf(cmd, cmdformat, checkconfig("notice_time"), magic_word);
    printf("%s\n",cmd);
    system(cmd);
}

void stoplistening()
{
    char * magic_word = "hbd check";
    char * cmdformat = "crontab -l | grep -v '%s' | crontab - ";
    char cmd[200];
    sprintf(cmd, cmdformat, magic_word);
    system(cmd);
}