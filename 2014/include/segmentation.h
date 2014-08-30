
#ifndef SEGMENTATION_H_INCLUDED
#define SEGMENTATION_H_INCLUDED

/*
#define ORANGE 0
#define YELLOW 2
#define BLUE 1
#define GREEN 3
#define WHITE 4
#define BLACK 5
#define NOCOLOR 6
*/
#define ORANGE 0
#define YELLOW 1
#define BLUE 2
#define GREEN 3
#define WHITE 4
#define BLACK 5
#define NOCOLOR 6

#define COLOR_COUNT 6

#define CMV_RBITS 6
#define CMV_RADIX (1 << CMV_RBITS)
#define CMV_RMASK (CMV_RADIX-1)


#define IS_COLOR(x, z) thres[0][data[x]] & thres[1][data[x + 1]] & thres[2][data[x + 2]] & (1<<z)
#define IS_AD_COLOR(x, z) ad_thres[0][data[x]] & ad_thres[1][data[x + 1]] & ad_thres[2][data[x + 2]] & (1<<z)


class run {
  public:
	short x, y, width;
	unsigned char color;
	int parent, next;
};

struct region {
	int color;
	int x1, y1, x2, y2;
	float cen_x, cen_y;
	int area;
	int run_start;
	int iterator_id;
	region *next;
};

struct color_class_state {
	region *list;
	int num;
	int min_area;
	unsigned char color;
	char *name;
};


class SEGMENTATION
{
  public:
	int width, height;
	unsigned char *data;
	unsigned char *th_data;

	run *rle;
	region *regions;
	color_class_state *colors;
	int run_c;
	int region_c;
	int max_runs;
	int max_reg;
	int max_area;

	SEGMENTATION( int width, int height );
	~SEGMENTATION();
	void readThresholds( const char path[100] );
	void thresholdImage( unsigned char *data );
	void EncodeRuns();
	void ConnectComponents();
	void ExtractRegions();
	void SeparateRegions();
	void SortRegions();
	region *SortRegionListByArea( region *list, int passes );
};


#endif
