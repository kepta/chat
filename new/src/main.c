#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <locale.h>

#include <string.h>
#include <pthread.h>

#include "lib/library.h"
#include "lib/audio.h"
#include "lib/video.h"
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

typedef struct {
    char *ipaddr;
    char *port;
} network_option_t;

void interrup_handler(int sig_num);
void all_shutdown(int signal);
void usage(FILE *stream);

void *audio_thread(void *args) {
  network_option_t *netopts = args;
  audio(netopts->ipaddr, netopts->port);
  return NULL;
}
int main( int argc, char *argv[]) {
    if (argc < 2) {
        usage(stderr);
        exit(1);
    }
    char *peer = argv[1];
    char *audio_port = "33033";
    char *video_port = "33034";

    vid_options_t vopt;
    int spawn_video = 0, print_error = 0;
    // int c;
    int width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;

    setlocale(LC_ALL, "");

    memset(&vopt, 0, sizeof(vid_options_t));
    vopt.width = DEFAULT_WIDTH;
    vopt.height = DEFAULT_HEIGHT;
    vopt.render_type = 0;
    vopt.refresh_rate = 20;
    vopt.saturation = -1.0;


    signal(SIGINT, all_shutdown);
    pthread_t thr;
    network_option_t netopts;
    netopts.ipaddr = peer;
    netopts.port = audio_port;
    pthread_create(&thr, NULL, audio_thread, (void *)&netopts);
    video(peer, video_port, &vopt);

    audio(peer, audio_port);
}

void interrup_handler(int sig_num) {
        signal(SIGINT, interrup_handler);
        printf("\n Termination \n");
        exit(1);
}
void audio_shutdown(int signal) {
  kill(getpid(), SIGUSR1);
}
void all_shutdown(int signal) {
  video_shutdown(signal);
  audio_shutdown(signal);
  kill(getpid(), SIGKILL);
  exit(0);
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
