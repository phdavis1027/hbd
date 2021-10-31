#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hbd.h"
#include <regex.h>
#define to "to"
#define on "on"

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

int main(int argc, char **argv)
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

        if(strcmp(*args,to) == 0){
            w = 1;
            rec = *(++args);
        }

        if(strcmp(*args, on) == 0){
            w = 1;
            date = *(++args);
            validate(date);
        }

        ++args;
    }

    if(w){
        if(!rec){
            printf("Whose birthday is it?");
            scanf("%s", inrec);
            rec = inrec;
        }

        if(!date){
            printf("When is the birthday?");
            scanf("%s", indate);
            validate(indate);
            date = indate;
        }

        writebday(rec, date);
    }

    retval = 0;

    return retval;
}

void writebday(const char *who, const char *when){

    FILE *bdays = fopen("./storage/bdays.txt","a");
    fprintf(bdays,"%s:%s\n",who,when);
    fclose(bdays);

}

int validate(const char *date){

    regex_t date_re;
    regmatch_t p_match[1];

    if(regcomp(&date_re, "((1)|(2)|(3)|(4)|(5)|(6)|(7)|(8)|(9)|(10)|(11)|(12))/(([0-2][0-9])|(3[01]))", REG_EXTENDED != 0)){
        printf("Oh no!\n");
    }

    int match = regexec(&date_re, date, 1, p_match, 0);

    if(match != 0)
        printf("No match\n");

    return 0;
}