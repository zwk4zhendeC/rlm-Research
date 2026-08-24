#include "license.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void heartbeatnotification(RLM_HANDLE ahandle, RLM_LICENSE alicense, int astatus)
{
    //even empty will cause a problem

    //this code below is not thread safe... but we are just testing. Plus without an RLM server it will not be called anyway
    (void)printf("\nheartbeatnotification : %d", astatus);
}

/*
 *  Configuration
 */


#define PRODUCT "actdemo"


int main(int argc, char *argv[])
{
    RLM_HANDLE rh = (RLM_HANDLE) NULL;
    RLM_LICENSE lic = (RLM_LICENSE) NULL;
    int stat;
    const char *product = PRODUCT;
    char input[RLM_MAX_LINE + 1];


    (void)printf("\nrlm_init");
    rh = rlm_init(".", argv[0], (char *)NULL);
    stat = rlm_stat(rh);
    (void)printf("\nrlm_init. Status : %d", stat);


    (void)printf("\nDo Auto Heartbeat (Y/N) ? (N) ");
    fgets(input, RLM_MAX_LINE, stdin);
    if (*input == 'y' || *input == 'Y')
    {
        (void)printf("\nrlm_auto_hb");
        stat = rlm_auto_hb(rh, (int)10, (int)1, heartbeatnotification);

        /*extern int      rlm_auto_hb(RLM_HANDLE, int, int,
          void(*func)(RLM_HANDLE, RLM_LICENSE, int));
          */
        (void)printf("\nrlm_auto_hb. Status : %d", stat);
    }

    (void)printf("\nRequest Option Set (Y/N) ? (Y) ");
    fgets(input, RLM_MAX_LINE, stdin);
    if (*input != 'n' && *input != 'N')
    {

        (void)printf("\nrlm_set_attr_req_opt");
        rlm_set_attr_req_opt(rh, "1408");
        stat = rlm_stat(rh);
        (void)printf("\nrlm_set_attr_req_opt  Status: %d", stat);
    }


    (void)printf("\nrlm_checkout");
    lic = rlm_checkout(rh, product, "1.0", 1);
    stat = rlm_license_stat(lic);
    (void)printf("\nrrlm_checkout  Status: %d", stat);

    (void)printf("\nEnter to finish: ");
    int x = fgetc(stdin);


    rlm_checkin(lic);
    rlm_close(rh);
}
