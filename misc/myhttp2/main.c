#include "httpd.h"

int main(int argc,char** argv)
{
    int i;
    int  port;
    char *credentials, *www_folder;
    char nocommands;
	config conf;


    port = htons(8080);
    credentials = NULL;
    www_folder = NULL;
    nocommands = 0;

    /* show all parameters for DBG purposes */
    for(i = 0; i < argc; i++) {
        DBG("argv[%d]=%s\n", i, argv[i]);
    }

    reset_getopt();
    while(1) {
        int option_index = 0, c = 0;
        static struct option long_options[] = {
            {"h", no_argument, 0, 0
            },
            {"help", no_argument, 0, 0},
            {"p", required_argument, 0, 0},
            {"port", required_argument, 0, 0},
            {"c", required_argument, 0, 0},
            {"credentials", required_argument, 0, 0},
            {"w", required_argument, 0, 0},
            {"www", required_argument, 0, 0},
            {"n", no_argument, 0, 0},
            {"nocommands", no_argument, 0, 0},
            {0, 0, 0, 0}
        };

        c = getopt_long_only(argc, argv, "", long_options, &option_index);

        /* no more options to parse */
        if(c == -1) break;

        /* unrecognized option */
        if(c == '?') {
            help();
            return 1;
        }

        switch(option_index) {
            /* h, help */
        case 0:
        case 1:
            DBG("case 0,1\n");
            help();
            return 1;
            break;

            /* p, port */
        case 2:
        case 3:
            DBG("case 2,3\n");
            port = htons(atoi(optarg));
            break;

            /* c, credentials */
        case 4:
        case 5:
            DBG("case 4,5\n");
            credentials = strdup(optarg);
            break;

            /* w, www */
        case 6:
        case 7:
            DBG("case 6,7\n");
            www_folder = malloc(strlen(optarg) + 2);
            strcpy(www_folder, optarg);
            if(optarg[strlen(optarg)-1] != '/')
                strcat(www_folder, "/");
            break;

            /* n, nocommands */
        case 8:
        case 9:
            DBG("case 8,9\n");
            nocommands = 1;
            break;
        }
    }

    conf.port = port;
    conf.credentials = credentials;
    conf.www_folder = www_folder;
    conf.nocommands = nocommands;

    OPRINT("www-folder-path...: %s\n", (www_folder == NULL) ? "disabled" : www_folder);
    OPRINT("HTTP TCP port.....: %d\n", ntohs(port));
    OPRINT("username:password.: %s\n", (credentials == NULL) ? "disabled" : credentials);
    OPRINT("commands..........: %s\n", (nocommands) ? "disabled" : "enabled");
	server_thread(conf);
    return 0;
}

