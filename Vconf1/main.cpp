#include "capture.h"
#include "r_video.h"

#include <stdlib.h>
#include <gtk/gtk.h>


GtkWidget *combo = NULL;
GtkWidget *field1 = NULL;
GtkWidget *field2 = NULL;
GtkWidget *field3 = NULL;
GtkWidget *field4 = NULL;
GtkWidget *reset_button = NULL;
GtkWidget *conf_button = NULL;
GtkWidget *color_checkbox = NULL;
GtkWidget *ball_checkbox = NULL;
GtkWidget *gate_checkbox = NULL;
GtkWidget *slider1 = NULL;
GtkWidget *slider2 = NULL;
GtkWidget *slider3 = NULL;
GtkWidget *slider4 = NULL;
GtkWidget *slider5 = NULL;
GtkWidget *slider6 = NULL;
GtkWidget *res_menu = NULL;
GtkWidget *fps_menu = NULL;
GtkWidget *fmt_menu = NULL;
GtkWidget *table2 = NULL;
GtkWidget *cam_conf = NULL;


GtkWidget *sliders[30];
GtkWidget *check_boxes[30];
GtkWidget *menus[30];
int slider_c = 0, check_boxes_c = 0, menus_c = 0;
int slider_ref[100];
int check_box_ref[100];
int menu_ref[100];

int res_val, fps_val, fmt_val;
int gw, gh;

char robot_dir[100];
char video_dev[30];


bool values_exist = false;
bool ball_det = false;
bool gate_det = false;
bool halt = false;


IMAGE_CONTEXT *video;
IMAGE_CONTEXT *thresholds;
uchar *frame;

CAM_SETTINGS cs;
SUPPORTED_SETTINGS ss;
CAMERA_CONTROLS cam_ctl[100];
char menu_names[32][100];
int cam_ctl_c = 0;
int menu_c = 0;

uchar thres[3][256];
uchar ad_thres[3][256];


bool if_end = false;
bool conf_window = false;
int color = ORANGE;
int bf, bc, tbf, tbc;


static gboolean idle_func(gpointer data);
void reset(GtkWidget *widget, gpointer data);
void update_menus(void);
void configure(GtkWidget *widget, gpointer data);
void exec_command(char *cmd, char *output);
void close_conf(void);
void end(void);




int main (int argc, char *argv[])
{
    GtkWidget *win = NULL;
    GtkWidget *table = NULL;
    GtkWidget *label = NULL;
    int vx, vy, tx, ty;
    char str[8];
    char path[100];
    FILE *in;



    in = fopen( "vconf", "r" );
    if( in ) {
        fscanf( in, "%s\n", video_dev );
        fscanf( in, "%s\n", robot_dir );
        fscanf( in, "%d %d %d %d\n", &vx, &vy, &tx, &ty );
        fclose( in );
    } else {
        printf( "Error. Could not find vconf file.\n" );
        exit(0);
    }

    memset( path, '\0', 100 );
    strcat( path, robot_dir );
    strcat( path, "conf1" );

    in = fopen( path, "r" );

    if( in ) {
        for( int i = 0; i < 3; i++ ) {
            for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &thres[i][j] );
            for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &ad_thres[i][j] );
        }

        fclose(in);
    } else {
        printf("Error. Can't open the threshold file\n");
        exit(0);
    }

    readThresholds( path );


    memset( path, '\0', 100 );
    strcat( path, robot_dir );
    strcat( path, "cam.sh" );

    system( path );


    // Initialize GTK+
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    // Create the main window
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (win), 8);
    gtk_window_set_title (GTK_WINDOW (win), "Vision Configurator");
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
    gtk_window_move(GTK_WINDOW (win), 220, 50);
    gtk_window_set_default_size (GTK_WINDOW (win), 600, 200);
    gtk_widget_realize (win);
    g_signal_connect (win, "destroy", end, NULL);


    // Creating a conteiner table with 3 rows and 7 columns
    table = gtk_table_new(3, 4, TRUE);
    gtk_container_add (GTK_CONTAINER (win), table);

    // Creating the dropdown menu
    combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Orange");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Yellow");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Blue");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Green");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "White");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Black");

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_table_attach_defaults(GTK_TABLE(table), combo, 0, 2, 0, 1);

    // Creating reset button
    reset_button = gtk_button_new_with_label( "Reset" );
    gtk_table_attach_defaults( GTK_TABLE(table), reset_button, 3, 4, 0, 1 );
    g_signal_connect( G_OBJECT(reset_button), "clicked", G_CALLBACK(reset), NULL );

    conf_button = gtk_button_new_with_label( "Configure" );
    gtk_table_attach_defaults( GTK_TABLE(table), conf_button, 3, 4, 1, 2 );
    g_signal_connect( G_OBJECT(conf_button), "clicked", G_CALLBACK(configure), NULL );


    // Creating labels
    label = gtk_label_new("Frame brushsize:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
    label = gtk_label_new("Colorspace brushsize:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 3, 4);
    label = gtk_label_new("Thres. frame brushsize:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 2, 3, 2, 3);
    label = gtk_label_new("Thres. colors. brushsize:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 2, 3, 3, 4);


    // Checkbox
    color_checkbox = gtk_check_button_new_with_label( "Additional color" );
    gtk_table_attach_defaults( GTK_TABLE(table), color_checkbox, 0, 1, 1, 2 );
    ball_checkbox = gtk_check_button_new_with_label( "Detect balls" );
    gtk_table_attach_defaults( GTK_TABLE(table), ball_checkbox, 2, 3, 0, 1 );
    gate_checkbox = gtk_check_button_new_with_label( "Detect gates" );
    gtk_table_attach_defaults( GTK_TABLE(table), gate_checkbox, 2, 3, 1, 2 );


    // Creating value fields
    field1 = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(field1), 3);
    sprintf(str, "%d\0", 1);
    gtk_entry_set_text(GTK_ENTRY(field1), str);
    gtk_table_attach_defaults(GTK_TABLE(table), field1, 1, 2, 2, 3);

    field2 = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(field2), 3);
    sprintf(str, "%d\0", 1);
    gtk_entry_set_text(GTK_ENTRY(field2), str);
    gtk_table_attach_defaults(GTK_TABLE(table), field2, 1, 2, 3, 4);

    field3 = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(field3), 3);
    sprintf(str, "%d\0", 1);
    gtk_entry_set_text(GTK_ENTRY(field3), str);
    gtk_table_attach_defaults(GTK_TABLE(table), field3, 3, 4, 2, 3);

    field4 = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(field4), 3);
    sprintf(str, "%d\0", 1);
    gtk_entry_set_text(GTK_ENTRY(field4), str);
    gtk_table_attach_defaults(GTK_TABLE(table), field4, 3, 4, 3, 4);


    gtk_widget_show_all( win );


    // Initializing capture
    open_device( video_dev );
    init_device( video_dev );
    start_capturing();

    get_camera_settings( &cs );
    get_camera_controls( cam_ctl, menu_names, &cam_ctl_c, &menu_c );
    get_supported_settings( cs, &ss );

    gw = cs.width;
    gh = cs.height;
    video = new_window( "Video", vx, vy, gw, gh );
    thresholds = new_window( "Thresholds", tx, ty, gw, gh );


    // GTK Main loop
    gtk_idle_add(idle_func, NULL);
    gtk_main();



    return 0;
}




static gboolean idle_func(gpointer data)
{
    char command[100];
    int posX, posY, x1, y1, x2, y2;
    int win_x, win_y;
    uchar **fr;


    while(1) {
        while(gtk_events_pending()) {
            if(gtk_main_iteration()) break;
        }

        if( if_end ) break;


        if( conf_window == true ) {
            memset( command, '\0', 100 );

            for( int i = 0; i < cam_ctl_c; i++ ) {
                if( cam_ctl[i].type == V4L2_CTRL_TYPE_INTEGER && cam_ctl[i].value != (int) gtk_range_get_value( GTK_RANGE(sliders[slider_ref[i]]) ) ) {
                    cam_ctl[i].value = (int) gtk_range_get_value( GTK_RANGE(sliders[slider_ref[i]]) );
                    set_cam_value( cam_ctl[i].id, cam_ctl[i].value );
                }
                if( cam_ctl[i].type == V4L2_CTRL_TYPE_BOOLEAN && cam_ctl[i].value != gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_boxes[check_box_ref[i]]) ) ) {
                    cam_ctl[i].value = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(check_boxes[check_box_ref[i]]) );
                    set_cam_value( cam_ctl[i].id, cam_ctl[i].value );
                }
                if( cam_ctl[i].type == V4L2_CTRL_TYPE_MENU && cam_ctl[i].value != gtk_combo_box_get_active( GTK_COMBO_BOX(menus[menu_ref[i]]) ) ) {
                    cam_ctl[i].value = gtk_combo_box_get_active( GTK_COMBO_BOX(menus[menu_ref[i]]) );
                    set_cam_value( cam_ctl[i].id, cam_ctl[i].value );
                }
            }

            if( fps_val != gtk_combo_box_get_active( GTK_COMBO_BOX(fps_menu) ) ) {
                fps_val = gtk_combo_box_get_active( GTK_COMBO_BOX(fps_menu) );

                stop_capturing();
                set_fps( ss.fps[fps_val] );
                start_capturing();
                cs.fps = ss.fps[fps_val];
            }
            if( res_val != gtk_combo_box_get_active( GTK_COMBO_BOX(res_menu) ) ) {
                res_val = gtk_combo_box_get_active( GTK_COMBO_BOX(res_menu) );

                stop_capturing();
                uninit_device();
                close_device();
                open_device( video_dev );
                XResizeWindow( video->display, video->window, ss.width[res_val], ss.height[res_val] );
                XResizeWindow( thresholds->display, thresholds->window, ss.width[res_val], ss.height[res_val] );
                image_create( video, ss.width[res_val], ss.height[res_val] );
                image_create( thresholds, ss.width[res_val], ss.height[res_val] );
                set_resolution( ss.width[res_val], ss.height[res_val] );

                init_device( video_dev );
                start_capturing();

                gw = ss.width[res_val];
                gh = ss.height[res_val];
                cs.width = gw;
                cs.height = gh;
                update_menus();
            }

            if( fmt_val != gtk_combo_box_get_active( GTK_COMBO_BOX(fmt_menu) ) ) {
                fmt_val = gtk_combo_box_get_active( GTK_COMBO_BOX(fmt_menu) );
                strcpy( cs.pix_fmt, ss.pix_fmt[fmt_val] );
                update_menus();
            }
        }


        // Hetkel on ainus implementeeritud piksliformaat YUYV ehk YUV422
        if( !halt ) {
            frame = read_frame();
            if( frame ) {

                if( strcmp( "YUYV", cs.pix_fmt ) == 0 ) set_working_frame_yuyv( frame, gw, gh );

                fr = get_working_frame();
            }
        }

        if( frame ) {

            if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(ball_checkbox) ) ) ball_det = true;
            else ball_det = false;
            if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(gate_checkbox) ) ) gate_det = true;
            else gate_det = false;



            // Testing ball and gate detection
            if( ball_det )
                detect_ball( &posX, &posY );
            if( gate_det ) {
                detect_gate( &posX, &posY, &x1, &y1, &x2, &y2, BLUE );
                detect_gate( &posX, &posY, &x1, &y1, &x2, &y2, YELLOW );
            }


            for(int i=0; i<3; i++) for(int j=0; j<256; j++) ad_thres[i][j] |= thres[i][j];

            bf = atoi( (char*) gtk_entry_get_text( GTK_ENTRY( field1 ) ) );
            bc = atoi( (char*) gtk_entry_get_text( GTK_ENTRY( field2 ) ) );
            tbf = atoi( (char*) gtk_entry_get_text( GTK_ENTRY( field3 ) ) );
            tbc = atoi( (char*) gtk_entry_get_text( GTK_ENTRY( field4 ) ) );

            color = gtk_combo_box_get_active( GTK_COMBO_BOX(combo) );

            if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(color_checkbox) ) ) {
                show_video( video, fr, ad_thres, color, bf, bc, &halt );
                show_threshold( thresholds, fr, ad_thres, color, tbf, tbc );
            } else {
                show_video( video, fr, thres, color, bf, bc, &halt );
                show_threshold( thresholds, fr, thres, color, tbf, tbc );
            }
        }



    }

    return FALSE;
}



void reset(GtkWidget *widget, gpointer data)
{
    color = gtk_combo_box_get_active( GTK_COMBO_BOX(combo) );

    if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(color_checkbox) ) ) {
        for( int i = 0; i < 3; i++ ) {
			for( int j = 0; j < 256; j++ ) ad_thres[i][j] = ( 0 );
//            for( int j = 0; j < 256; j++ ) ad_thres[i][j] &= ~( 1 << color );
        }
    } else {
        for( int i = 0; i < 3; i++ ) {
			for( int j = 0; j < 256; j++ ) thres[i][j] = ( 0);	
//            for( int j = 0; j < 256; j++ ) thres[i][j] &= ~( 1 << color );
        }
    }
}



void update_menus()
{
    char cur_res[32];


    get_supported_settings( cs, &ss );

    gtk_widget_destroy( res_menu );
    res_menu = gtk_combo_box_text_new();
    for( int i = 0; i < ss.res_c; i++ ) {
        memset( cur_res, '\0', sizeof(cur_res) );
        sprintf( cur_res, "%d x %d", ss.width[i], ss.height[i] );
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT(res_menu), cur_res );

        if( ss.width[i] == cs.width && ss.height[i] == cs.height )
            res_val = i;
    }
    gtk_combo_box_set_active( GTK_COMBO_BOX(res_menu), res_val );
    gtk_table_attach_defaults( GTK_TABLE(table2), res_menu, 1, 2, 0, 1 );


    gtk_widget_destroy( fps_menu );
    fps_menu = gtk_combo_box_text_new();
    for( int i = 0; i < ss.fps_c; i++ ) {
        memset( cur_res, '\0', sizeof(cur_res) );
        sprintf( cur_res, "%d fps", ss.fps[i] );
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT(fps_menu), cur_res );

        if( ss.fps[i] == cs.fps )
            fps_val = i;
    }
    gtk_combo_box_set_active( GTK_COMBO_BOX(fps_menu), fps_val );
    gtk_table_attach_defaults( GTK_TABLE(table2), fps_menu, 1, 2, 1, 2 );

    gtk_widget_show_all( cam_conf );
}



void configure(GtkWidget *widget, gpointer data)
{
    GtkWidget *label = NULL;

    char cur_res[32];
    conf_window = true;



    cam_conf = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER(cam_conf), 8);
    gtk_window_set_title( GTK_WINDOW(cam_conf), "Configure camera" );
    gtk_window_set_position (GTK_WINDOW(cam_conf), GTK_WIN_POS_CENTER);
    gtk_window_move(GTK_WINDOW(cam_conf), 220, 50);
    gtk_window_set_default_size (GTK_WINDOW(cam_conf), 400, 200);
    gtk_widget_realize( cam_conf );
    g_signal_connect( cam_conf, "destroy", close_conf, NULL );

    table2 = gtk_table_new( cam_ctl_c+3, 4, TRUE );
    gtk_container_add ( GTK_CONTAINER(cam_conf), table2 );



    label = gtk_label_new( "Resolution" );
    gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5 );
    gtk_table_attach_defaults( GTK_TABLE(table2), label, 0, 1, 0, 1 );

    label = gtk_label_new( "Frame rate" );
    gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5 );
    gtk_table_attach_defaults( GTK_TABLE(table2), label, 0, 1, 1, 2 );

    update_menus();


    // Pixel format menu
    label = gtk_label_new( "Pixel format" );
    gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5 );
    gtk_table_attach_defaults( GTK_TABLE(table2), label, 0, 1, 2, 3 );

    fmt_menu = gtk_combo_box_text_new();
    for( int i = 0; i < ss.fmt_c; i++ ) {
        memset( cur_res, '\0', sizeof(cur_res) );
        if( strcmp( ss.pix_fmt[i], "YUYV" ) == 0 ) sprintf( cur_res, "%s", ss.pix_fmt[i] );
        else sprintf( cur_res, "%s (not implemented)", ss.pix_fmt[i] );
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT(fmt_menu), cur_res );

        if( strcmp( ss.pix_fmt[i], cs.pix_fmt ) == 0 )
            fmt_val = i;
    }
    gtk_combo_box_set_active( GTK_COMBO_BOX(fmt_menu), fmt_val );
    gtk_table_attach_defaults( GTK_TABLE(table2), fmt_menu, 1, 2, 2, 3 );




    for( int i = 0; i < cam_ctl_c; i++ ) {

        label = gtk_label_new( cam_ctl[i].name );
        gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5 );
        gtk_table_attach_defaults( GTK_TABLE(table2), label, 0, 1, i+3, i+4 );

        if( cam_ctl[i].type == V4L2_CTRL_TYPE_INTEGER ) {

            sliders[slider_c] = gtk_hscale_new_with_range( cam_ctl[i].min_value, cam_ctl[i].max_value, cam_ctl[i].step );
            gtk_scale_set_draw_value( GTK_SCALE(sliders[slider_c]), FALSE );
            gtk_range_set_value( GTK_RANGE(sliders[slider_c]), cam_ctl[i].value );
            gtk_table_attach_defaults( GTK_TABLE(table2), sliders[slider_c], 1, 4, i+3, i+4 );

            slider_ref[i] = slider_c;
            slider_c++;
        }

        if( cam_ctl[i].type == V4L2_CTRL_TYPE_BOOLEAN ) {
            check_boxes[check_boxes_c] = gtk_check_button_new();
            gtk_table_attach_defaults( GTK_TABLE(table2), check_boxes[check_boxes_c], 1, 2, i+3, i+4 );

            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(check_boxes[check_boxes_c]), cam_ctl[i].value );

            check_box_ref[i] = check_boxes_c;
            check_boxes_c++;
        }

        if( cam_ctl[i].type == V4L2_CTRL_TYPE_MENU ) {

            menus[menus_c] = gtk_combo_box_text_new();
            for( int j = cam_ctl[i].menu_start; j < cam_ctl[i].menu_end; j++ )
                gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT(menus[menus_c]), menu_names[j] );

            gtk_combo_box_set_active( GTK_COMBO_BOX(menus[menus_c]), cam_ctl[i].value );
            gtk_table_attach_defaults( GTK_TABLE(table2), menus[menus_c], 1, 2, i+3, i+4 );

            menu_ref[i] = menus_c;
            menus_c++;
        }
    }


    gtk_widget_show_all( cam_conf );
    values_exist = true;
}



void exec_command(char *cmd, char *output)
{
    FILE* pipe = popen( cmd, "r" );
    if ( !pipe ) {
        printf("Error. Can't open the pipe.\n");
        return;
    }

    while( !feof( pipe ) ) {
        fgets( output, 128, pipe );
    }

    pclose( pipe );
}



void close_conf()
{
    conf_window = false;
}




void end()
{
    Window cr;
    FILE *out;
    char path[100];
    int vx, vy, tx, ty;

    memset( path, '\0', 100 );
    strcat( path, robot_dir );
    strcat( path, "conf1" );

    // Creating the threshold file
    out = fopen( path, "w" );
    if( out ) {
        for( int i = 0; i < 3; i++ ) {
            for( int j = 0; j < 256; j++ ) fprintf( out, "%c", thres[i][j] );
            for( int j = 0; j < 256; j++ ) fprintf( out, "%c", ad_thres[i][j] );
        }
        fclose( out );
    }

    memset( path, '\0', 100 );
    strcat( path, robot_dir );
    strcat( path, "cam.sh" );

    out = fopen( path, "w" );
    if( out ) {
        for( int i = 0; i < cam_ctl_c; i++ ) {
            fprintf( out, "v4l2-ctl -d %s -c %s=%d\n", video_dev, cam_ctl[i].format_name, cam_ctl[i].value );
        }
        fprintf( out, "v4l2-ctl --set-fmt-video=width=%d,height=%d,pixelformat=%d\n", cs.width, cs.height, fmt_val );
        fprintf( out, "v4l2-ctl -p %d\n", cs.fps );

        fclose( out );
    }

    memset( path, '\0', 100 );
    strcat( path, "chmod 777 " );
    strcat( path, robot_dir );
    strcat( path, "cam.sh" );

    system( path );



    // Reading current window coordinates and saving them
    XTranslateCoordinates( video->display, video->window, RootWindowOfScreen(video->screen),
                           0, 0, &vx, &vy, &cr );
    XTranslateCoordinates( thresholds->display, thresholds->window, RootWindowOfScreen(thresholds->screen),
                           0, 0, &tx, &ty, &cr );

    memset( path, '\0', 100 );
    strcat( path, robot_dir );
    strcat( path, "vconf" );

    out = fopen( path, "w" );
    if( out ) {
        fprintf( out, "%s\n", video_dev );
        fprintf( out, "%s\n", robot_dir );
        // minus 22 is a hack, cause I'm obvioysly doing smth wrong, still, my y-coordinates always increase by 22
        fprintf( out, "%d %d %d %d\n", vx, vy-22, tx, ty-22 );
        fclose( out );
    }


    gtk_main_quit();
    video_cleanup( video, thresholds );
    r_video_cleanup();
    if_end = true;
}
