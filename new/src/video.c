#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include "lib/library.h"
#include "lib/display.h"
#include "lib/video.h"

#define GET_HEIGHT(h) (h)
#define GET_WIDTH(w) (((w)/8)*8)

#define COLOR_DEPTH       3
#define BANDWIDTH_BUFLEN  64

static connection_t *con;
// static size_t conslen;
// static pthread_mutex_t conslock;
static CvCapture* cv_cap;
static int disp_bandwidth = 0;
static int width;
static int height;
static int depth = COLOR_DEPTH;
static int render_type;

static void callback(connection_t *con, void *data, size_t length) {

  unsigned long index = ntohl(((unsigned long*)data)[0]);
  int y = index;
  draw_line(&(((char*)data)[(sizeof(unsigned long))]), length - (sizeof(unsigned long)), y, depth);

  // if (disp_bandwidth) {
  //   char bandstr[BANDWIDTH_BUFLEN];
  //   memset(bandstr, 0, BANDWIDTH_BUFLEN);
  //   sprintf(bandstr, " Bandwidth : %f MB/s", 1000 * p2p_bandwidth());
  //   write_bandwidth(bandstr, strlen(bandstr), width, height);
  // }
}


void video_shutdown(int signal) {
  cvReleaseCapture( &cv_cap );
  end_screen();
}

// Use our do listen version here.
static void *dolisten(void *args) {
    int socket;
    int port = atoi((char *)args);
    init_udp (port, &socket);
    receive_messages( callback, socket, width * height * depth);
    return NULL;
}

int video(char *peer, char *port, vid_options_t *vopt) {

  width = GET_WIDTH(vopt->width);
  height = GET_HEIGHT(vopt->height);
  render_type = vopt->render_type;
  disp_bandwidth = vopt->disp_bandwidth;

  display_options_t dopt;
  memset(&dopt, 0, sizeof(display_options_t));

  dopt.intensity_threshold = vopt->intensity_threshold;
  dopt.saturation = vopt->saturation;
  dopt.monochrome = vopt->monochrome;
  dopt.r = vopt->r;
  dopt.g = vopt->g;
  dopt.b = vopt->b;
  dopt.ascii_values = vopt->ascii_values;

  init_screen(&dopt);
  curs_set(0);
  // pthread_mutex_init(&conslock, NULL);

  // cons = calloc(1, sizeof(connection_t));
  con = malloc(sizeof(connection_t));
  // Use our p2p connect version here!
  if (connect_udp(peer, port, con)) {
    fprintf(stderr, "Unable to connect to server.\n");
  }
  else {
    printf("Video connection established\n");
  }

  pthread_t thr;
  pthread_create(&thr, NULL, &dolisten, (void *)port);

  IplImage* color_img;
  IplImage* resize_img = cvCreateImage(cvSize(width, height), 8, 3);  // Creates an image of size(width,height) and bit depth (8 bits to store 256 colors) and 3 channels(R,G,B for color display)!

  cv_cap = cvCaptureFromCAM(0);     // Zero to use the first webcam. For multiple webcams different argument.
  char line_buffer[sizeof(unsigned long) + width * depth];
  struct timespec tim, actual_tim;
  tim.tv_sec = 0;
  tim.tv_nsec = (1000000000 - 1) / vopt->refresh_rate;


  while (1) {
    /* Get each frame */

    color_img = cvQueryFrame(cv_cap);   // Get's the last filled frame present in the buffer! Frame by frame input.
    if(color_img && resize_img) {
      cvResize(color_img, resize_img, CV_INTER_AREA);     // Resizing image using CV_INTER_AREA interpolation technique.

      unsigned long line_index;
      for (line_index = 0; line_index < (resize_img->imageSize / (width * depth)); line_index++) {

        memset(line_buffer, 0, sizeof(line_buffer));
        unsigned long send_index = htonl(line_index);   // From host byte order to network byte order.

        memcpy(line_buffer, &send_index, sizeof(unsigned long));
        memcpy(&(line_buffer[sizeof(unsigned long)]), resize_img->imageData + (line_index * width * depth), width * depth);

        /*
          REPLACE WITH OUT CODE HERE. FOR THE P2P SENDING PROCESS!
        */

        //p2p_broadcast(&cons, &conslen, &conslock, line_buffer, + sizeof(line_buffer));
        // p2p_broadcast(&cons, &conslen, &conslock, line_buffer, + sizeof(line_buffer));
        udp_send(con, line_buffer, sizeof(line_buffer));
      }
      nanosleep(&tim, &actual_tim);  // Suspends execution for time.tv_nsec. Used for nano second precision.
    }
  }

  /*
    PERFORM CLEAN UP!
   */

  cvReleaseCapture(&cv_cap);
  end_screen();

  return 0;
}
