#include "capture.h"

#include "iostream"
#include "cstdlib"


Capture::Capture(){
    this->fd = -1;
    this->xset = 0;
    this->yset = 0;
}

Capture::~Capture(){
    /*if(this->buffers){
        delete this->buffers;
    }*/
}

IMAGE_CONTEXT *Capture::new_window( char *wnd_name, int x, int y, int width, int height )
{
    IMAGE_CONTEXT *img_ctx;
    img_ctx = new IMAGE_CONTEXT;

    g_width = width;
    g_height = height;

    // First we are creating the window
    if ( ( img_ctx->display = XOpenDisplay(NULL) ) == NULL )
    {
        printf("Error: XOpenDisplay failed.\n");
        exit(1);
    }

    img_ctx->screenNumber = DefaultScreen( img_ctx->display );
    img_ctx->screen = XScreenOfDisplay( img_ctx->display, img_ctx->screenNumber );


    img_ctx->window = XCreateSimpleWindow (
			img_ctx->display,
			RootWindowOfScreen( img_ctx->screen ),
			0,
			0,
			g_width,
			g_height,
			0,
			BlackPixelOfScreen( img_ctx->screen ),
			BlackPixelOfScreen( img_ctx->screen )
    );


    img_ctx->xImage = NULL;
    if( this->image_create( img_ctx, g_width, g_height ) < 0 )
    {
        printf("Error: image_create() failed\n");
        exit(1);
    }

    XSelectInput( img_ctx->display, img_ctx->window, ButtonPressMask | KeyPressMask );
    XMapRaised( img_ctx->display, img_ctx->window );
    XStoreName( img_ctx->display, img_ctx->window, wnd_name );
	XGetWindowAttributes( img_ctx->display, img_ctx->window, &(img_ctx->windowAttributes ) );

	XMoveWindow( img_ctx->display, img_ctx->window, x, y );


    return img_ctx;
}



// Reading the data from memory, converting it to RGB, and then showing the picture
void Capture::show_video( IMAGE_CONTEXT *image_ctx, uchar **frame)
{
	XImage *xImage1 = image_ctx->xImage;
	XEvent event;

	uchar Y, U, V;
	uchar R, G, B;
	int pix_c = 0;


	uchar *imageLine1 = (uchar*) xImage1 -> data;


	for( int i = 0; i < g_height; i++ ) {
	    for( int j = 0; j < 3*g_width; j += 3 ) {

            Y = frame[i][ j + 0 ];
            U = frame[i][ j + 1 ];
            V = frame[i][ j + 2 ];

            yuv_to_rgb( Y, U, V, &R, &G, &B );
            imageLine1[ 4*pix_c + 0 ] = R;
            imageLine1[ 4*pix_c + 1 ] = G;
            imageLine1[ 4*pix_c + 2 ] = B;
            imageLine1[ 4*pix_c + 3 ] = 255;


            pix_c++;
	    }
	}

	image_put( image_ctx );

	XPending( image_ctx->display);
}



void Capture::show_threshold( IMAGE_CONTEXT *image_ctx, uchar **frame, uchar thres[3][256], int color)
{
    XImage *xImage1 = image_ctx->xImage;
    XEvent event;
	uchar Y, U, V;
	int pix_c = 0;
	bool col;
	int by_s, by_e, bx_s, bx_e, ys, ye, us, ue, vs, ve;

	uchar *imageLine1 = (uchar*) xImage1 -> data;


    for( int i = 0; i < g_height; i++ ) {
	    for( int j = 0; j < 3*g_width; j += 3 ) {

            Y = frame[i][ j + 0 ];
            U = frame[i][ j + 1 ];
            V = frame[i][ j + 2 ];

            col = isColor( thres, Y, U, V, color );

            if( col ) for(int i=0; i<4; i++) imageLine1[ 4*pix_c + i ] = 255;
            else for(int i=0; i<4; i++) imageLine1[ 4*pix_c + i ] = 0;


            pix_c++;
	    }
	}


	image_put( image_ctx );



    XPending( image_ctx->display );
}



bool Capture::isColor( uchar thres[3][256], uchar Y, uchar U, uchar V, int color )
{
    return thres[0][Y] & thres[1][U] & thres[2][V] & ( 1 << color );
}




// Convert a pixel from YUV to RGB
void Capture::yuv_to_rgb( uchar y, uchar u, uchar v, uchar *r, uchar *g, uchar *b )
{
	int amp = 255;
	double R, G, B;

	//conversion equations
	B = amp * ( 0.004565*y + 0.000001*u + 0.006250*v - 0.872 );
	G = amp * ( 0.004565*y - 0.001542*u - 0.003183*v + 0.531 );
	R = amp * ( 0.004565*y + 0.007935*u - 1.088 );

	//R, G and B must be in the range from 0 to 255
	if( R < 0 ) R = 0;
	if( G < 0 ) G = 0;
	if( B < 0 ) B = 0;

	if( R > 255 ) R = 255;
	if( G > 255 ) G = 255;
	if( B > 255 ) B = 255;

	*r = (uchar) R;
	*g = (uchar) G;
	*b = (uchar) B;
}





int Capture::image_put( IMAGE_CONTEXT *img_ctx )
{

	if( img_ctx->xImage == NULL ) return -1;

	XShmPutImage( img_ctx->display, img_ctx->window, img_ctx->gc, img_ctx->xImage, 0, 0, 0, 0, g_width, g_height, false );

	return 0;
}





int Capture::image_create( IMAGE_CONTEXT *img_ctx, int width, int height )
{
	XGCValues          gcValues;
	ulong              gcValuesMask;
	XWindowAttributes  windowAttributes;


	/*if ( img_ctx->xImage != NULL )
        image_destroy( img_ctx );*/

    g_width = width;
    g_height = height;


	gcValues.function = GXcopy;
	gcValuesMask = GCFunction;


	// Creating a graphics context
	img_ctx->gc = XCreateGC( img_ctx->display, img_ctx->window, gcValuesMask, &gcValues );
	XGetWindowAttributes( img_ctx->display, img_ctx->window, &windowAttributes );

	img_ctx->visual = windowAttributes.visual;
	img_ctx->depth = windowAttributes.depth;

	if( XShmQueryExtension( img_ctx->display ) )
        img_ctx->isShared = 1;
	else
	{
		img_ctx->isShared = 0;
		printf("Error. isShared = FALSE\n");
	}

	errno = 0;
	img_ctx->xImage = NULL;
	img_ctx->sharedPixmap = None;

	img_ctx->shmInfo.shmid = -1;
	img_ctx->shmInfo.shmaddr = NULL;


	if( ( img_ctx->xImage = XShmCreateImage( img_ctx->display, img_ctx->visual,
		img_ctx->depth, ZPixmap, NULL, &( img_ctx->shmInfo ), g_width, g_height ) ) == NULL )
	{
	    printf("Error. Failed to create image.\n");
		return -1;
	}

	if( ( img_ctx->shmInfo.shmid = shmget( IPC_PRIVATE,
		img_ctx->xImage->bytes_per_line * img_ctx->xImage->height,
		IPC_CREAT | 0777 ) ) < 0 )
	{
	    printf("Error. Failed to create a segment.\n");
		return -1;
	}

	if( ( img_ctx->shmInfo.shmaddr = (char *) shmat(img_ctx->shmInfo.shmid, 0, 0 ) ) < 0 )
	{
		img_ctx->shmInfo.shmaddr = NULL;
		return -1;
	}

	img_ctx->xImage->data = img_ctx->shmInfo.shmaddr;
	img_ctx->shmInfo.readOnly = false;
	if ( !XShmAttach( img_ctx->display, &( img_ctx->shmInfo ) ) )
	{
        printf("Error. Attach failed.\n");
		return -1;
	}

	return 0;
}




int Capture::image_destroy( IMAGE_CONTEXT *img_ctx )
{
	if ( img_ctx->xImage == NULL ) return 0;

	if( img_ctx->isShared )
	{
		if( img_ctx->shmInfo.shmid >= 0 )
		{
			XShmDetach( img_ctx->display, &( img_ctx->shmInfo ) );
			shmdt( img_ctx->shmInfo.shmaddr );
			img_ctx->shmInfo.shmaddr = NULL;
			shmctl( img_ctx->shmInfo.shmid, IPC_RMID, 0 );
			img_ctx->shmInfo.shmid = -1;
		}
	}

	img_ctx->xImage->data = NULL;
	XDestroyImage( img_ctx->xImage );
	img_ctx->xImage = NULL;


	if( img_ctx->sharedPixmap != None )
	{
		XFreePixmap( img_ctx->display, img_ctx->sharedPixmap );
		img_ctx->sharedPixmap = None;
	}

	if( img_ctx->display != NULL )
	{
		XFreeGC( img_ctx->display, img_ctx->gc );
		XDestroyWindow( img_ctx->display, img_ctx->window );
		img_ctx->display = NULL;
	}

	return 0;
}





uchar *Capture::read_frame()
{    
    v4l2_buffer buf;
    memset( &buf, 0, sizeof(buf) );
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    
    if( -1 == xioctl( fd, VIDIOC_DQBUF, &buf ) ) {
        switch (errno) {
            case EAGAIN:
                return NULL;

            default:
                fprintf( stderr, "VIDIOC_DQBUF error %d, %s\n", errno, strerror(errno) );
                exit( EXIT_FAILURE );
        }
	}

	last_buf = buf.index;


    if ( -1 == xioctl( fd, VIDIOC_QBUF, &buf ) ) {
        fprintf( stderr, "VIDIOC_QBUF error %d, %s\n", errno, strerror(errno) );
        exit( EXIT_FAILURE );
    }
        
    return (uchar*) buffers[ last_buf ].start;
}




void Capture::start_capturing()
{
    enum v4l2_buf_type type;
    
    for( int i = 0; i < N_BUFFERS; i++ ) {

            v4l2_buffer buf;
            memset( &buf, 0, sizeof(buf) );

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if( -1 == xioctl( fd, VIDIOC_QBUF, &buf ) ) {
                fprintf( stderr, "VIDIOC_QBUF error %d, %s\n", errno, strerror(errno) );
                exit( EXIT_FAILURE );
            }

    }
    
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if( -1 == xioctl( fd, VIDIOC_STREAMON, &type ) ) {
        fprintf( stderr, "VIDIOC_STREAMON error %d, %s\n", errno, strerror(errno) );
        exit( EXIT_FAILURE );
    }
}



void Capture::stop_capturing()
{
    enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if ( -1 == xioctl( fd, VIDIOC_STREAMOFF, &type ) ) {
        fprintf( stderr, "VIDIOC_STREAMOFF error %d, %s\n", errno, strerror(errno) );
        exit( EXIT_FAILURE );
    }
}




void Capture::init_mmap( char *dev_name )
{
    v4l2_requestbuffers req;

    memset( &req, 0, sizeof(req) );

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if ( -1 == xioctl( fd, VIDIOC_REQBUFS, &req ) ) {
        if ( EINVAL == errno ) {
                fprintf( stderr, "%s does not support memory mapping\n", dev_name );
                exit( EXIT_FAILURE );
        }
    }


    buffers = (BUFFER*) calloc( req.count, sizeof(*buffers) );
    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }


    for( unsigned int i = 0; i < req.count; i++ ) {

        v4l2_buffer buf;
        memset( &buf, 0, sizeof(buf) );

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = i;

        if( -1 == xioctl( fd, VIDIOC_QUERYBUF, &buf ) ) {
            fprintf( stderr, "VIDIOC_QUERYBUF error %d, %s\n", errno, strerror(errno) );
            exit( EXIT_FAILURE );
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap( NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset );

        if ( MAP_FAILED == buffers[i].start ) {
            fprintf( stderr, "MemoryMap error %d, %s\n", errno, strerror(errno) );
            exit( EXIT_FAILURE );
        }
    }

}




void Capture::init_device( char *dev_name )
{
    v4l2_capability cap;
    v4l2_format fmt;

    unsigned int min;
    std::cout << "Initializing :" << dev_name << std::endl;

    if( -1 == xioctl( fd, VIDIOC_QUERYCAP, &cap ) ) {
        if( EINVAL == errno ) {
            fprintf(stderr, "%s is no V4L2 device\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            fprintf( stderr, "VIDIOC_QUERYCAP error %d, %s\n", errno, strerror(errno) );
            exit( EXIT_FAILURE );
        }
    }

    if( !( cap.capabilities & V4L2_CAP_VIDEO_CAPTURE ) ) {
        fprintf(stderr, "%s is no video capture device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    if( !( cap.capabilities & V4L2_CAP_STREAMING ) ) {
        fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
        exit(EXIT_FAILURE);
    }


    memset( &fmt, 0, sizeof(fmt) );

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if( -1 == xioctl( fd, VIDIOC_G_FMT, &fmt ) ) {
        fprintf( stderr, "VIDIOC_G_FMT error %d, %s\n", errno, strerror(errno) );
        exit( EXIT_FAILURE );
    }


    // Buggy driver paranoia.
    min = fmt.fmt.pix.width * 2;
    if( fmt.fmt.pix.bytesperline < min )
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if( fmt.fmt.pix.sizeimage < min )
        fmt.fmt.pix.sizeimage = min;

    init_mmap( dev_name );
}



void Capture::uninit_device()
{
    for( int i = 0; i < N_BUFFERS; i++ )
        if ( -1 == munmap( buffers[i].start, buffers[i].length ) ) {
            fprintf( stderr, "Memory unmap error %d, %s\n", errno, strerror(errno) );
            exit( EXIT_FAILURE );
        }

    free( buffers );
}





void Capture::open_device( char *dev_name )
{
    struct stat st;
    std::cout << "Opening :" << dev_name << std::endl;
    
    if( -1 == stat( dev_name, &st ) ) {
        fprintf( stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno) );
        exit( EXIT_FAILURE );
    }

    if( !S_ISCHR( st.st_mode ) ) {
        fprintf( stderr, "%s is no device\n", dev_name );
        exit( EXIT_FAILURE );
    }

    fd = open( dev_name, O_RDWR | O_NONBLOCK, 0 );

    if( -1 == fd ) {
        fprintf( stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno) );
        exit( EXIT_FAILURE );
    }
}



void Capture::close_device()
{
    std::cout << "Closing device!" << std::endl;
    if ( -1 == close( fd ) ) {
        fprintf( stderr, "Device Close error %d, %s\n", errno, strerror(errno) );
        exit( EXIT_FAILURE );
    }

    fd = -1;
}




void Capture::get_camera_controls( CAMERA_CONTROLS cam_ctl[100], char menu_names[30][100], int *cam_ctl_c, int *menu_c )
{
    struct v4l2_queryctrl queryctrl;
	int id;


	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
	while (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == 0) {
		if (queryctrl.type != V4L2_CTRL_TYPE_CTRL_CLASS &&
		    !(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
			enter_control_options( &queryctrl, cam_ctl, menu_names, cam_ctl_c, menu_c );
		}
		queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
	}
	if (queryctrl.id != V4L2_CTRL_FLAG_NEXT_CTRL)
		return;
	for (id = V4L2_CID_USER_BASE; id < V4L2_CID_LASTP1; id++) {
		queryctrl.id = id;
		if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == 0 &&
		    !(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
			enter_control_options( &queryctrl, cam_ctl, menu_names, cam_ctl_c, menu_c );
		}
	}
	for (queryctrl.id = V4L2_CID_PRIVATE_BASE;
			ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == 0; queryctrl.id++) {
		if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
			enter_control_options( &queryctrl, cam_ctl, menu_names, cam_ctl_c, menu_c );
		}
	}
}


void Capture::enter_control_options( struct v4l2_queryctrl *queryctrl, CAMERA_CONTROLS cam_ctl[100], char menu_names[30][100], int *cam_ctl_c, int *menu_c )
{
    struct v4l2_control control;
    struct v4l2_querymenu querymenu;
    std::string s;


    strcpy( cam_ctl[*cam_ctl_c].name, (char*) queryctrl->name );
    s = name2var( (unsigned char*) cam_ctl[*cam_ctl_c].name );
    strcpy( cam_ctl[*cam_ctl_c].format_name, s.c_str() );

    cam_ctl[*cam_ctl_c].id = queryctrl->id;
    cam_ctl[*cam_ctl_c].type = queryctrl->type;
    cam_ctl[*cam_ctl_c].max_value = queryctrl->maximum;
    cam_ctl[*cam_ctl_c].min_value = queryctrl->minimum;
    cam_ctl[*cam_ctl_c].step = queryctrl->step;

    memset( &control, 0, sizeof(control) );
    control.id = queryctrl->id;

    if( 0 == ioctl( fd, VIDIOC_G_CTRL, &control ) )
    cam_ctl[*cam_ctl_c].value = control.value;

    if( queryctrl->type == V4L2_CTRL_TYPE_MENU ) {
        cam_ctl[*cam_ctl_c].menu_start = *menu_c;
        memset( &querymenu, 0, sizeof(querymenu) );
        querymenu.id = queryctrl->id;

        for( querymenu.index = queryctrl->minimum;
            querymenu.index <= queryctrl->maximum;
            querymenu.index++ ) {
            if( 0 == ioctl ( fd, VIDIOC_QUERYMENU, &querymenu ) ) {
                strcpy( menu_names[*menu_c], (char*) querymenu.name );
                (*menu_c)++;
            }
        }
        cam_ctl[*cam_ctl_c].menu_end = *menu_c;

    }

    (*cam_ctl_c)++;
}




void Capture::set_cam_value( int id, int val )
{
    struct v4l2_control control;
    memset( &control, 0, sizeof(control) );

    control.id = id;
    control.value = val;
    ioctl( fd, VIDIOC_S_CTRL, &control );
}




void Capture::get_camera_settings( CAM_SETTINGS *cs )
{
    struct v4l2_format vfmt;
    struct v4l2_streamparm parm;

    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if( ioctl( fd, VIDIOC_G_FMT, &vfmt ) == 0) {
        cs->width = vfmt.fmt.pix.width;
        cs->height = vfmt.fmt.pix.height;
        strcpy( cs->pix_fmt, fcc2s( vfmt.fmt.pix.pixelformat ).c_str() );
    }
    if( ioctl( fd, VIDIOC_G_PARM, &parm ) == 0 ) {
        const struct v4l2_fract &tf = parm.parm.capture.timeperframe;
        cs->fps = tf.denominator / tf.numerator;
    }
}



std::string Capture::fcc2s(unsigned int val)
{
	std::string s;

	s += val & 0xff;
	s += (val >> 8) & 0xff;
	s += (val >> 16) & 0xff;
	s += (val >> 24) & 0xff;
	return s;
}




void Capture::get_supported_settings( CAM_SETTINGS cs, SUPPORTED_SETTINGS *ss )
{
	struct v4l2_fmtdesc fmt;
	struct v4l2_frmsizeenum frmsize;
	struct v4l2_frmivalenum frmival;
	int res_c = 0;
	int fps_c = 0;

	fmt.index = 0;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	while ( ioctl( fd, VIDIOC_ENUM_FMT, &fmt ) >= 0 ) {
	    strcpy( ss->pix_fmt[fmt.index], fcc2s(fmt.pixelformat).c_str() );

	    if( strcmp( ss->pix_fmt[fmt.index], cs.pix_fmt ) == 0 ) {

            frmsize.pixel_format = fmt.pixelformat;
	        frmsize.index = 0;
	        while( ioctl( fd, VIDIOC_ENUM_FRAMESIZES, &frmsize ) >= 0 ) {
	            if ( frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE ) {

	                ss->width[frmsize.index] = frmsize.discrete.width;
                    ss->height[frmsize.index] = frmsize.discrete.height;

                    if( ss->width[frmsize.index] == cs.width && ss->height[frmsize.index] == cs.height ) {

                        frmival.pixel_format = fmt.pixelformat;
                        frmival.width = frmsize.discrete.width;
                        frmival.height = frmsize.discrete.height;
                        frmival.index = 0;

                        while( ioctl( fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival ) >= 0 ) {
                            ss->fps[frmival.index] = frmival.discrete.denominator / frmival.discrete.numerator;
                            frmival.index++;
                        }
                        ss->fps_c = frmival.index;
                    }

	            } else
                    printf("Error. Framesize type is not V4L2_FRMSIZE_TYPE_DISCRETE (unimplemented)\n");


	            frmsize.index++;
	        }
	        ss->res_c = frmsize.index;
	    }

	    fmt.index++;
	}
	ss->fmt_c = fmt.index;
}




void Capture::set_fps( int fps )
{
    struct v4l2_streamparm parm;

    memset( &parm, 0, sizeof(parm) );
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.timeperframe.numerator = 1000;
    parm.parm.capture.timeperframe.denominator = fps * 1000;

    if( ioctl( fd, VIDIOC_S_PARM, &parm ) != 0 )
        fprintf( stderr, "set_fps error %d, %s\n", errno, strerror(errno) );
}


void Capture::set_resolution( int width, int height )
{
    struct v4l2_format in_vfmt;
    in_vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if ( ioctl( fd, VIDIOC_G_FMT, &in_vfmt ) == 0 ) {
        in_vfmt.fmt.pix.width = width;
        in_vfmt.fmt.pix.height = height;

        if( ioctl( fd, VIDIOC_S_FMT, &in_vfmt ) != 0 )
            fprintf( stderr, "set_resolution error %d, %s\n", errno, strerror(errno) );
    }

}





std::string Capture::name2var(unsigned char *name)
{
	std::string s;
	int add_underscore = 0;

	while (*name) {
		if (isalnum(*name)) {
			if (add_underscore)
				s += '_';
			add_underscore = 0;
			s += std::string(1, tolower(*name));
		}
		else if (s.length()) add_underscore = 1;
		name++;
	}
	return s;
}





int Capture::xioctl( int fh, int request, void *arg )
{
    int r;
    do {
        r = ioctl( fh, request, arg );
    } while ( -1 == r && EINTR == errno );

    return r;
}



void Capture::video_cleanup( IMAGE_CONTEXT *v1, IMAGE_CONTEXT *v2 )
{
    stop_capturing();
    uninit_device();
    close_device();

    image_destroy( v1 );
    image_destroy( v2 );
}





