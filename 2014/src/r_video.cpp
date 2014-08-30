#define PI 3.14159265

#define MIN_BALL_AREA 1
#define MIN_GATE_AREA 300

#define IS_COLOR(x, y, z) y_array[data[y][x]] & u_array[data[y][x + 1]] & v_array[data[y][x + 2]] & (1<<z)
#define IS_AD_COLOR(x, y, z) y_ad_array[data[y][x]] & u_ad_array[data[y][x + 1]] & v_ad_array[data[y][x + 2]] & (1<<z)


#include "r_video.h"
#include "capture.h"




char y_array[255];
char u_array[255];
char v_array[255];
char y_ad_array[255];
char u_ad_array[255];
char v_ad_array[255];


uchar **data = NULL;

int g_min_x, g_max_x;
int g_min_y, g_max_y;
int b_min_x, b_max_x;
int b_min_y, b_max_y;
int gate_min_low[ 2000 / 5 ];
int correct_c;

int gwidth, gheight;



void black_ch()
{
    for(int i=gheight-1; i>=0; i--) for(int j=gwidth-3; j>=0; j--)
    {
        if( IS_COLOR( 3*j, i, YELLOW ) ) {
            data [i][3*j + 0 ] = 0;
            data [i][3*j + 1 ] = 255;
            data [i][3*j + 2 ] = 0;
        }
    }
}


void detect_gate( int *x, int *y, int *x1, int *y1, int *x2, int *y2, int color )
{
    int g_area;
    float aspect;
    double color_coef;

    *x = -1;
    *y = -1;


    for( int i = 0; i < gwidth-1; i += 20 ) {

        for( int j = gheight-1; j >= 0; j -= 20 ) {

            if( IS_AD_COLOR( 3*i, j, color ) ) {

                g_min_x = gwidth;
                g_max_x = 0;
                g_min_y = gheight;
                g_max_y = 0;

                correct_c = 0;

                memset( gate_min_low, 0, sizeof(gate_min_low) );

                g_area = find_gate_area( i, j, color  );
                if( g_min_y != g_max_y ) aspect = (float) (g_max_x - g_min_x) / (g_max_y - g_min_y);
                else aspect = 0.0f;


                if( g_area > 20 && aspect > 0.5f && aspect < 7.0 && ( (double)correct_c / (double)g_area ) > 0.025 ) {

                    if( color == BLUE ) color_coef = 1.5;
                    else color_coef = 2.0;

                    double min_area;
                    if(g_max_y < 200) min_area = (0.3289*pow((double)g_max_y, 1.6197)) / (1.5*color_coef);
                    else min_area = (0.3289*pow((double)g_max_y, 1.6197)) / (4.0*color_coef);

                    double max_area = (0.3289*pow((double)g_max_y, 1.6197)) * 1.2;
                    printf("%f\n", min_area);
                    if( g_area < min_area || g_area > max_area ) break;


                    *x = g_min_x + ( g_max_x - g_min_x ) / 2;
                    *y = g_max_y;

                    *x1 = g_min_x;
                    *y1 = gate_min_low[ g_min_x / 5 + 2 ];
                    *x2 = g_max_x;
                    *y2 = gate_min_low[ g_max_x / 5 - 2 ];


                    draw_gate_lines();
                    draw_dot( *x, *y );
                    if( *x1 > 5 && *x1 < gwidth - 5 && *y1 > 5 && *y1 < gheight - 5 ) draw_dot( *x1, *y1 );
                    if( *x2 > 5 && *x2 < gwidth - 5 && *y2 > 5 && *y2 < gheight - 5 ) draw_dot( *x2, *y2 );
                    return;

                } else
                    break;

            }

            if( !(IS_COLOR( 3*i, j, GREEN )) && !(IS_COLOR( 3*i, j, WHITE )) && !(IS_COLOR( 3*i, j, BLACK )) ) break;

            /*data[j][3*i+0] = 255;
            data[j][3*i+1] = 255;
            data[j][3*i+2] = 255;*/

        }
    }

}


void draw_gate_lines()
{
    for(int i = g_min_x; i < g_max_x; i++) {
        data[ g_min_y ] [ 3*i + 0 ] = 255;
        data[ g_min_y ] [ 3*i + 1 ] = 255;
        data[ g_min_y ] [ 3*i + 2 ] = 255;
        data[ g_max_y ] [ 3*i + 0 ] = 255;
        data[ g_max_y ] [ 3*i + 1 ] = 255;
        data[ g_max_y ] [ 3*i + 2 ] = 255;
    }
    for(int i = g_min_y; i < g_max_y; i++) {
        data[ i ] [ 3*g_min_x + 0 ] = 255;
        data[ i ] [ 3*g_min_x + 1 ] = 255;
        data[ i ] [ 3*g_min_x + 2 ] = 255;
        data[ i ] [ 3*g_max_x + 0 ] = 255;
        data[ i ] [ 3*g_max_x + 1 ] = 255;
        data[ i ] [ 3*g_max_x + 2 ] = 255;
    }
}


void draw_rect(int x, int y, int width, int height)
{
    for(int i = x; i < x + width; i++) {
        data[ y ] [ 3*i + 0 ] = 255;
        data[ y ] [ 3*i + 1 ] = 255;
        data[ y ] [ 3*i + 2 ] = 255;
        data[ y + height ] [ 3*i + 0 ] = 255;
        data[ y + height ] [ 3*i + 1 ] = 255;
        data[ y + height ] [ 3*i + 2 ] = 255;
    }
    for(int i = y; i < y + height; i++) {
        data[ i ] [ 3*x + 0 ] = 255;
        data[ i ] [ 3*x + 1 ] = 255;
        data[ i ] [ 3*x + 2 ] = 255;
        data[ i ] [ 3*(x+width) + 0 ] = 255;
        data[ i ] [ 3*(x+width) + 1 ] = 255;
        data[ i ] [ 3*(x+width) + 2 ] = 255;
    }
}




void draw_dot( int x, int y )
{
    data[ y ] [ 3*x + 0 ] = 255;
    data[ y ] [ 3*x + 1 ] = 0;
    data[ y ] [ 3*x + 2 ] = 255;
    data[ y ] [ 3*(x-1) + 0 ] = 255;
    data[ y ] [ 3*(x-1) + 1 ] = 0;
    data[ y ] [ 3*(x-1) + 2 ] = 255;
    data[ y ] [ 3*(x+1) + 0 ] = 255;
    data[ y ] [ 3*(x+1) + 1 ] = 0;
    data[ y ] [ 3*(x+1) + 2 ] = 255;
    data[ y-1 ] [ 3*x + 0 ] = 255;
    data[ y-1 ] [ 3*x + 1 ] = 0;
    data[ y-1 ] [ 3*x + 2 ] = 255;
    data[ y+1 ] [ 3*x + 0 ] = 255;
    data[ y+1 ] [ 3*x + 1 ] = 0;
    data[ y+1 ] [ 3*x + 2 ] = 255;

}




bool check_for_line( int x, int y, int b_min_x, int b_max_x )
{
	float m;
	float b;
	int new_x, new_y;
	int last[5] = { 0, 0, 0, 0, 0 };
	int cur = 0;
	int white_c, black_c;
	int cur_x;
	bool gr_chk = false;



    for( int k = 0; k < 3; k++ ) {

        if( k == 0 ) cur_x = x;
        if( k == 1 ) cur_x = b_min_x;
        if( k == 2 ) cur_x = b_max_x;

        // Leian joone võrrandi
        if( y == gheight ) return false;
        m = (float) ( gwidth/2 - cur_x ) / (float) ( gheight - y );
        b = (float) cur_x - m*(float)y;

        gr_chk = false;

        for( int i = y; i < gheight; i++ ) {

            black_c = 0;
            white_c = 0;

            new_y = i;
            new_x = int ( m*i + b );

            if( !gr_chk ){
                if ( IS_COLOR( 3*new_x, new_y, GREEN ) ) gr_chk = true;
            }
            else {

                if( IS_COLOR( 3*new_x, new_y, WHITE ) ) last[cur] = WHITE;
                else if( IS_COLOR( 3*new_x, new_y, BLACK ) ) last[cur] = BLACK;
                else last[cur] = 0;

                if( cur < 4 ) cur++;
                else cur = 0;

                for( int j = 0; j < 5; j++ ) {
                    if( last[j] == WHITE ) white_c++;
                    else if( last[j] == BLACK ) black_c++;
                }

                if( black_c > 0 && white_c > 0 ) return true;
            }
        }
    }

	return false;
}




int find_ball_area( int x, int y, int *green_c )
{
	int area = 1;


    if( IS_COLOR( 3*x, y, ORANGE ) ) correct_c++;

    if( x < b_min_x ) b_min_x = x;
	if( x > b_max_x ) b_max_x = x;
	if( y < b_min_y ) b_min_y = y;
	if( y > b_max_y ) b_max_y = y;

	// Märgime läbitud pikslid ära
	data[ y ] [ 3*x + 0 ] = 255;
	data[ y ] [ 3*x + 1 ] = 0;
	data[ y ] [ 3*x + 2 ] = 0;

	// Laieneme rekursiivselt kõrvalasuvatesse samavärvi pikslitesse
	if( x > 0 && IS_AD_COLOR( 3*(x-1), y,  ORANGE ) )		    area += find_ball_area( x-1, y, green_c );
	if( x < gwidth-1 && IS_AD_COLOR( 3*(x+1), y,  ORANGE ) )        area += find_ball_area( x+1, y, green_c );
	if( y > 0 && IS_AD_COLOR( 3*x, y-1,  ORANGE ) )  		    area += find_ball_area( x, y-1, green_c );
	if( y < gheight-1 && IS_AD_COLOR( 3*x, y+1,  ORANGE ) )	    area += find_ball_area( x, y+1, green_c );

	// Kontrollime rohelisi piksleid
	if( x > 30 && IS_COLOR( 3*(x-10), y , GREEN ) ) (*green_c)++;
	if( x < gwidth-30 && IS_COLOR( 3*(x+10), y , GREEN ) ) (*green_c)++;
	if( y > 10 && IS_COLOR( 3*x, y-10 , GREEN ) ) (*green_c)++;


	return area;
}


int find_gate_area( int x, int y, int color )
{
	int area = 1;

	// Väravatuvastuse jaoks vajalik, nende järgi saab keskpunkti leida
	if( x < g_min_x ) g_min_x = x;
	if( x > g_max_x ) g_max_x = x;
	if( y < g_min_y ) g_min_y = y;
	if( y > g_max_y ) g_max_y = y;

	if( y > gate_min_low[ x / 5 ] ) gate_min_low[ x / 5 ] = y;


    if( IS_COLOR( 3*x, y, color ) ) correct_c++;

	// Märgime läbitud pikslid ära
	data[ y ] [ 3*x + 0 ] = 255;
	data[ y ] [ 3*x + 1 ] = 0;
	data[ y ] [ 3*x + 2 ] = 0;


	// Laieneme rekursiivselt kõrvalasuvatesse samavärvi pikslitesse
	if( x > 4 && IS_AD_COLOR( 3*(x-5), y,  color ) )			area += find_gate_area( x-5, y, color );
	if( x < gwidth-5 && IS_AD_COLOR( 3*(x+5), y,  color ) )		area += find_gate_area( x+5, y, color );
	if( y > 4 && IS_AD_COLOR( 3*x, y-5,  color ) )				area += find_gate_area( x, y-5, color );
	if( y < gheight-5 && IS_AD_COLOR( 3*x, y+5,  color ) )		area += find_gate_area( x, y+5, color );

	return area;
}



// Funktsioon kõige lähemal asuva palli leidmiseks
// int *x, int *y - tagastame leitud palli x- ja y-koordinaadid
void detect_ball( int *x, int *y )
{
	int skip[32] = { 1, 1, 1, 2, 4, 5, 6, 6, 7, 9, 11, 12, 14, 16, 18, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 25, 25, 25, 25, 25, 5 };
	int cur_skip = 1;
    int area;
    int green_c = 0;
    int pix_c = 0;


	*x = -1;	// Kui palli ei leita, siis jäävad need väärtused
	*y = -1;


	for( int i = gheight-1; i >= 0; i -= cur_skip ) {
		cur_skip = skip[i / 20];

	    for( int j = gwidth-1; j >= 0; j -= cur_skip )
        {
            pix_c++;

            if( IS_AD_COLOR( 3*j, i, ORANGE ) )
            {
                b_min_x = gwidth;
                b_max_x = 0;
                b_min_y = gheight;
                b_max_y = 0;

                correct_c = 0;
                area = find_ball_area( j, i, &green_c );
                printf("%d\n", area);


                if( area >= MIN_BALL_AREA  && green_c > 2 && ( (double)correct_c / (double)area ) > 0.4 ) {

                    if( check_for_line( j, i, b_min_x, b_max_x ) ) {
                        printf("Line detected\n");
                        continue;
                    } else
                        printf("No line\n");

					int last_point = j;
					int first_point = j;
					for( int k = j; k >= 0; k -= cur_skip ) {
						if( data[ i ] [ 3*k + 0 ] == 255 && data[ i ] [ 3*k + 1 ] == 0 && data[ i ] [ 3*k + 2 ] == 0 ) first_point = k;
					}
					for( int k = j; k < gwidth; k += cur_skip ) {
					    if( data[ i ] [ 3*k + 0 ] == 255 && data[ i ] [ 3*k + 1 ] == 0 && data[ i ] [ 3*k + 2 ] == 0 ) last_point = k;
					}



                    *x = first_point + ( last_point - first_point ) / 2;
                    *y = i;

                    draw_rect( b_min_x, b_min_y, b_max_x-b_min_x, b_max_y-b_min_y );

                    i = 0; j = 0;
                }
            }



            /*data[ i ] [ 3*j + 0 ] = 255;
            data[ i ] [ 3*j + 1 ] = 0;
            data[ i ] [ 3*j + 2 ] = 255;*/

        }
	}

	//printf("%f\%\n", (double)pix_c/(640.0*480.0)*100.0);

}


void get_rgb_frame(uchar **frame, uchar *rgb_frame, int g_height, int g_width){
    uchar Y, U, V;
	uchar R, G, B;
	int pix_c = 0;

	for( int i = 0; i < g_height; i++ ) {
	    for( int j = 0; j < 3*g_width; j += 3 ) {

            Y = frame[i][ j + 0 ];
            U = frame[i][ j + 1 ];
            V = frame[i][ j + 2 ];

            yuv_to_rgb( Y, U, V, &R, &G, &B );
            rgb_frame[ 4*pix_c + 0 ] = Y;
            rgb_frame[ 4*pix_c + 1 ] = U;
            rgb_frame[ 4*pix_c + 2 ] = Y;
            rgb_frame[ 4*pix_c + 3 ] = V;

            pix_c++;
	    }
	}

}


void set_working_frame_yuyv( uchar *frame, int width, int height)
{
    uchar Y1, Y2, U, V;
    int pix_c = 0;
    int row_id = 0;
    int total;


    if( gwidth != width || gheight != height ) {
        if( data ) {
            for( int i = 0; i < gheight; i++ )
                delete data[i];
            delete data;
        }

        gwidth = width;
        gheight = height;

        data = new uchar*[height];
        for( int i = 0; i < height; i++ )
            data[i] = new uchar[3*width];
    }


    for( int i = 0; i < 2*gwidth*gheight; i += 4 ) {

        Y1 = frame[ i + 0 ];
        U = frame[ i + 1 ];
        Y2 = frame[ i + 2 ];
        V = frame[ i + 3 ];
        data [row_id] [ 3*pix_c + 0 ] = Y1;
        data [row_id] [ 3*pix_c + 1 ] = U;
        data [row_id] [ 3*pix_c + 2 ] = V;

        data [row_id] [ 3*(pix_c+1) + 0 ] = Y2;
        data [row_id] [ 3*(pix_c+1) + 1 ] = U;
        data [row_id] [ 3*(pix_c+1) + 2 ] = V;

        pix_c += 2;
        if( pix_c == gwidth ) {
            pix_c = 0;
            row_id++;
        }
    }
}




uchar **get_working_frame()
{
    return data;
}




void readThresholds( char path[100] )
{
    FILE *in = fopen( path, "r" );
    if( !in ) {
        printf( "Conf faili ei leitud.\n" );
        exit(0);
    }


    memset( y_array, 0, 255 );
    memset( u_array, 0, 255 );
    memset( v_array, 0, 255 );
    memset( y_ad_array, 0, 255 );
    memset( u_ad_array, 0, 255 );
    memset( v_ad_array, 0, 255 );


    for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &y_array[j] );
    for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &y_ad_array[j] );
    for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &u_array[j] );
    for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &u_ad_array[j] );
    for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &v_array[j] );
    for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &v_ad_array[j] );

    fclose(in);
}




void get_real_coord(int *x, int *y)
{
    int x1 = *x;
    int y1 = *y;

    *y = ( -4822.55 / ( ( 480 - y1 ) - 491.398 ) - 11.4457 ) / ( 0.2424727 ) + 23;
    *x = -33.2481*( x1 - 320 ) / ( -11.3981 - y1 );
}


void r_video_cleanup()
{
    for( int i = 0; i < gheight; i++ )
        delete data[i];
    delete data;
}




