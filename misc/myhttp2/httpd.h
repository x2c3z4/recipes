#include <pthread.h>
#include "debug.h"
#define IO_BUFFER 256
#define BUFFER_SIZE 1024


/*
 * Standard header to be send along with other header information like mimetype.
 *
 * The parameters should ensure the browser does not cache our answer.
 * A browser should connect for each file and not serve files from his cache.
 * Using cached pictures would lead to showing old/outdated pictures
 * Many browser seem to ignore, or at least not always obey those headers
 * since i observed caching of files from time to time.
 */
#define STD_HEADER "Connection: close\r\n" \
    "Server: MJPG-Streamer/0.2\r\n" \
    "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n" \
    "Pragma: no-cache\r\n" \
    "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n"

/*
 * Maximum number of server sockets (i.e. protocol families) to listen.
 */
#define MAX_SD_LEN 50

/*
 * Only the following fileypes are supported.
 *
 * Other filetypes are simply ignored!
 * This table is a 1:1 mapping of files extension to a certain mimetype.
 */
static const struct {
    const char *dot_extension;
    const char *mimetype;
} mimetypes[] = {
    { ".html", "text/html" },
    { ".htm",  "text/html" },
    { ".css",  "text/css" },
    { ".js",   "text/javascript" },
    { ".txt",  "text/plain" },
    { ".jpg",  "image/jpeg" },
    { ".jpeg", "image/jpeg" },
    { ".png",  "image/png"},
    { ".gif",  "image/gif" },
    { ".ico",  "image/x-icon" },
    { ".swf",  "application/x-shockwave-flash" },
    { ".cab",  "application/x-shockwave-flash" },
    { ".jar",  "application/java-archive" },
    { ".json", "application/json" }
};

/* the webserver determines between these values for an answer */
typedef enum {
    A_UNKNOWN,
    A_SNAPSHOT,
    A_STREAM,
    A_COMMAND,
    A_FILE,
    A_INPUT_JSON,
    A_OUTPUT_JSON,
    A_PROGRAM_JSON,
} answer_t;

/*
 * the client sends information with each request
 * this structure is used to store the important parts
 */
typedef struct {
    answer_t type;
    char *parameter;
    char *client;
    char *credentials;
} request;

/* the iobuffer structure is used to read from the HTTP-client */
typedef struct {
    int level;              /* how full is the buffer */
    char buffer[IO_BUFFER]; /* the data */
} iobuffer;

/* store configuration for each server instance */
typedef struct {
    int port;
    char *credentials;
    char *www_folder;
    char nocommands;
} config;

/* context of each server thread */
typedef struct {
    int sd[MAX_SD_LEN];
    int sd_len;
    pthread_t threadID;

    config conf;
} context;

/*
 * this struct is just defined to allow passing all necessary details to a worker thread
 * "cfd" is for connected/accepted filedescriptor
 */
typedef struct {
    context *pc;
    int fd;
} cfd;

/* prototypes */
void *server_thread(void *arg);
void send_error(int fd, int which, char *message);
void send_Output_JSON(int fd, int plugin_number);
void send_Input_JSON(int fd, int plugin_number);
void send_Program_JSON(int fd);
