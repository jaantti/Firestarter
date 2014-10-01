#ifndef CAPTURE_H_INCLUDED
#define CAPTURE_H_INCLUDED



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <string>


#define N_BUFFERS 4

#define ORANGE 0
#define YELLOW 1
#define BLUE 2
#define GREEN 3
#define WHITE 4
#define BLACK 5



typedef unsigned char uchar;

struct BUFFER {
    void *start;
    size_t length;
};


typedef struct
{
	Display         *display;
	GC              gc;
	Visual          *visual;
	int             depth;
	Bool            isShared;
	XImage          *xImage;
	Pixmap          sharedPixmap;
	XShmSegmentInfo shmInfo;

	XEvent               *event;
	Window               window;
	int	                 screenNumber;
	Atom                 atomWMDeleteWindow;
	Screen               *screen;
	Colormap             colormap;
	XWindowAttributes    windowAttributes;

} IMAGE_CONTEXT;


struct CAMERA_CONTROLS
{
    int id;
    char name[32];
    char format_name[32];
    int type;
    int value;
    int min_value;
    int max_value;
    int step;
    int menu_start;
    int menu_end;
};

struct CAM_SETTINGS
{
    int fps;
    char pix_fmt[15];
    int width;
    int height;
};

struct SUPPORTED_SETTINGS
{
    int fps[100];
    char pix_fmt[15][30];
    int width[100];
    int height[100];

    int fps_c;
    int fmt_c;
    int res_c;
};




IMAGE_CONTEXT *new_window(char *wnd_name, int x, int y, int width, int height);
void show_video(IMAGE_CONTEXT *image_ctx, uchar **frame, uchar thres[3][256], int color, int bf, int bc, bool *halt);
void show_threshold(IMAGE_CONTEXT *image_ctx, uchar **frame, uchar thres[3][256], int color, int bf, int bc);
bool isColor(uchar thres[3][256], uchar Y, uchar U, uchar V, int color);
void yuv_to_rgb(uchar y, uchar u, uchar v, uchar *r, uchar *g, uchar *b);
int image_put(IMAGE_CONTEXT *img_ctx);
int image_create(IMAGE_CONTEXT *img_ctx, int width, int height);
int image_destroy(IMAGE_CONTEXT *img_ctx);

uchar *read_frame(void);
void start_capturing(void);
void stop_capturing(void);
void init_mmap(char *dev_name);
void init_device(char *dev_name);
void uninit_device(void);
void open_device(char *dev_name);
void close_device(void);
void start_capturing(void);
void get_camera_controls(CAMERA_CONTROLS cam_ctl[100], char menu_names[30][100], int *cam_ctl_c, int *menu_c);
void enter_control_options(struct v4l2_queryctrl *queryctrl, CAMERA_CONTROLS cam_ctl[100], char menu_names[30][100], int *cam_ctl_c, int *menu_c);
void get_supported_settings(CAM_SETTINGS cs, SUPPORTED_SETTINGS *ss);
void get_camera_settings(CAM_SETTINGS *cs);
static std::string name2var(unsigned char *name);
static std::string fcc2s(unsigned int val);
void set_cam_value(int id, int val);
void set_fps(int fps);
void set_resolution(int width, int height);
int xioctl(int fh, int request, void *arg);
void video_cleanup(IMAGE_CONTEXT *v1, IMAGE_CONTEXT *v2);



#endif // CAPTURE_H_INCLUDED
