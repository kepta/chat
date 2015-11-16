#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>

#include "library.h"
#include "audio.c"
// #include "video.c"
typedef struct {
    char *ipaddr;
    char *port;
} network_option_t;

void interrup_handler(int sig_num);
void usage(FILE *stream);

int main( int argc, char *argv[]) {
    if (argc < 2) {
        usage(stderr);
        exit(1);
    }
    char *peer = argv[1];
    char *audio_port = "33033";
    char *video_port = "33034";
    audio(peer, audio_port);
}

void interrup_handler(int sig_num) {
        signal(SIGINT, interrup_handler);
        printf("\n Termination \n");
        exit(1);
}

void usage(FILE *stream) {
  fprintf(stream,
    "Usage: p2pvc [-h] [server] [options]\n"
    "A point to point color terminal video chat.\n"
    "\n"
    "  -v    Enable video chat.\n"
    "  -d    Dimensions of video in either [width]x[height] or [width]:[height]\n"
    "  -A    Audio port.\n"
    "  -V    Video port.\n"
    "  -b    Display incoming bandwidth in the top-right of the video display.\n"
    "  -e    Print stderr (which is by default routed to /dev/null).\n"
    "  -c    Use a specified color (i.e green is 0:100:0).\n"
    "  -B    Render in Braille.\n"
    "  -I    Set threshold for braille.\n"
    "  -E    Use an edge filter.\n"
    "  -a    Use custom ascii to print the video.\n"
    "\n"
    "Report bugs to https://github.com/mofarrell/p2pvc/issues.\n"
  );
}
