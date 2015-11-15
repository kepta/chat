#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <p2plib.h>
#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include <fcntl.h>
#include <inttypes.h>

// #include<client.h>
// #include<server.h>

#include <audio.h>
#include <video.h>

#define DEFAULT_WIDTH 100
#define DEFAULT_HEIGHT 40

typedef struct {
  char *ipaddr;
  char *port;
} network_options_t;


void all_shutdown(int signal) {
  video_shutdown(signal);
  audio_shutdown(signal);
  kill(getpid(), SIGKILL);
  exit(0);
}

void usage(FILE *stream) {
  fprintf(stream,
     "Usage: ./start [ip address]
     Enter a valid ip address to connect to!"
  );
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage(stderr);
    exit(1);
  }

  char *addr = argv[1];
  char *audio_port = "55555";
  char *video_port = "55556";
  
  vid_options_t vopt;             //

  int width = DEFAULT_WIDTH;
  int height = DEFAULT_HEIGHT;

  setlocale(LC_ALL, "");
  memset(&vopt, 0, sizeof(vid_options_t));
 
    vopt.width = DEFAULT_WIDTH;
    vopt.height = DEFAULT_HEIGHT;
    vopt.render_type = 0;
    vopt.refresh_rate = 20;
    vopt.saturation = -1.0;

    signal(SIGINT, all_shutdown);
    network_options_t netopts;
    netopts.ipaddr = addr;
    netopts.port = audio_port;

    // Start both audio and video.
    start_audio(addr, audio_port);
    start_video(addr, video_port, &vopt);
    
  return 0;
}

