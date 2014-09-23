#include "segmentation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <cv.h>
#include <highgui.h>
#include <unistd.h>
#include <sstream>
#include <cstdlib>
#include <sys/time.h>
#include <thread>

#include <iostream>

unsigned char thres[3][256];
unsigned char ad_thres[3][256];

SEGMENTATION::SEGMENTATION( int width, int height ) {
	this->width = width;
	this->height = height;
	this->max_runs = width*height/1;
	this->max_reg = width*height;

	this->data = NULL;
	this->th_data = new unsigned char[ width*height ];
	this->rle = new run[ max_runs ];
        this->run_c = 0;
	this->regions = new region[ max_reg ];
	this->region_c = 0;
	this->max_area = 0;
	this->colors = new color_class_state[ COLOR_COUNT ];
}


SEGMENTATION::~SEGMENTATION() {
	delete this->th_data;
	if( this->rle )
		delete this->rle;
	if( this->regions )
		delete this->regions;
	if( this->colors )
		delete this->colors;
}


void SEGMENTATION::readThresholds( const char path[100] )
{
    FILE *in = fopen( path, "r" );
    if( !in ) {
        printf( "VisionConf faili ei leitud.\n" );
        exit(0);
    }

	for( int i = 0; i < 3; i++ ) {
		memset( thres[i], 0, 255 );
		memset( ad_thres[i], 0, 255 );
	}

    for( int i = 0; i < 3; i++ ) {
		for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &thres[i][j] );
		for( int j = 0; j < 256; j++ ) fscanf( in, "%c", &ad_thres[i][j] );
	}

    fclose( in );
}


void SEGMENTATION::thresholdImage( unsigned char *data )
{
	int pix_c = 0;
	unsigned char col1, col2, col3;
	unsigned char c1, c2;
	unsigned char Y, Y2, U, V;
	this->data = data;
        bool T1, T2, T3, T4, T5, T6;
        for( int i = 0; i < 2 * this->width * this->height; i += 4 ) {
            /*
            std::cout << "Y is :" << (int)data[i] << std::endl;
            std::cout << "U is :" << (int)data[i+1] << std::endl;
            std::cout << "Y3 is :" << (int)data[i+2] << std::endl;
            std::cout << "V is :" << (int)data[i+3] << std::endl;
            */
            Y = data[i];
            U = data[i+1];
            Y2 = data[i+2];
            V = data[i+3];
            
            T1 = thres[0][Y] & thres[1][U] & thres[2][V] & ( 1 << GREEN );
            T2 = thres[0][Y] & thres[1][U] & thres[2][V] & ( 1 << WHITE );
            T3 = thres[0][Y] & thres[1][U] & thres[2][V] & ( 1 << ORANGE );
            T4 = thres[0][Y] & thres[1][U] & thres[2][V] & ( 1 << YELLOW );
            T5 = thres[0][Y] & thres[1][U] & thres[2][V] & ( 1 << BLUE );
            T6 = thres[0][Y] & thres[1][U] & thres[2][V] & ( 1 << BLACK );

            if(T1) c1 = GREEN;
            else if(T2) c1 = WHITE;
            else if(T3) c1 = ORANGE;
            else if(T4) c1 = YELLOW;
            else if(T5) c1 = BLUE;
            else if(T6) c1 = BLACK;
            else c1 = NOCOLOR;

            T1 = thres[0][Y2] & thres[1][U] & thres[2][V] & ( 1 << GREEN );
            T2 = thres[0][Y2] & thres[1][U] & thres[2][V] & ( 1 << WHITE );
            T3 = thres[0][Y2] & thres[1][U] & thres[2][V] & ( 1 << ORANGE );
            T4 = thres[0][Y2] & thres[1][U] & thres[2][V] & ( 1 << YELLOW );
            T5 = thres[0][Y2] & thres[1][U] & thres[2][V] & ( 1 << BLUE );
            T6 = thres[0][Y2] & thres[1][U] & thres[2][V] & ( 1 << BLACK );

            if(T1) c2 = GREEN;
            else if(T2) c2 = WHITE;
            else if(T3) c2 = ORANGE;
            else if(T4) c2 = YELLOW;
            else if(T5) c2 = BLUE;
            else if(T6) c2 = BLACK;
            else c2 = NOCOLOR;

            this->th_data[ pix_c ] = c1;
            this->th_data[ pix_c +1 ] = c2;
            pix_c+=2;
	}
}


void SEGMENTATION::EncodeRuns()
// Changes the flat array version of the thresholded image into a run
// length encoded version, which speeds up later processing since we
// only have to look at the points where values change.
{
  unsigned char m, save;
  unsigned char *row = NULL;
  int x, y, j, l;
  run r;
  unsigned char *map = this->th_data;
  run *rle = this->rle;

  r.next = 0;

  // initialize terminator restore
  save = map[0];


  j = 0;
  for(y = 0; y < this->height; y++){
    row = &map[y * this->width];

    // restore previous terminator and store next
    // one in the first pixel on the next row
    row[0] = save;
    if(y!=height-1){
        save = row[this->width];
        row[this->width] = 255;
    }
    r.y = y;

    x = 0;
    while(x < this->width){
      m = row[x];
      r.x = x;

      l = x;
      while(row[x] == m) x++;

      if( m != NOCOLOR || x >= this->width ) {
		r.color = m;
		r.width = x - l;
		r.parent = j;
		rle[j++] = r;


		if(j >= this->max_runs) {
        	row[this->width] = save;

			printf( "Runlength buffer exceeded.\n" );
			this->run_c = j;
        	return;
        }
      }
    }
  }

  this->run_c = j;
}


void SEGMENTATION::ConnectComponents()
// Connect components using four-connecteness so that the runs each
// identify the global parent of the connected region they are a part
// of.  It does this by scanning adjacent rows and merging where
// similar colors overlap.  Used to be union by rank w/ path
// compression, but now it just uses path compression as the global
// parent index, a simpler rank bound in practice.
// WARNING: This code is complicated.  I'm pretty sure it's a correct
//   implementation, but minor changes can easily cause big problems.
//   Read the papers on this library and have a good understanding of
//   tree-based union find before you touch it
{
  int l1, l2;
  run r1, r2;
  int i, j, s;
  int num = this->run_c;
  run *map = this->rle;


  // l2 starts on first scan line, l1 starts on second
  l2 = 0;
  l1 = 1;
  while(map[l1].y == 0) l1++; // skip first line

  // Do rest in lock step
  r1 = map[l1];
  r2 = map[l2];
  s = l1;
  while(l1 < num){

    if(r1.color==r2.color && r1.color){
      // case 1: r2.x <= r1.x < r2.x + r2.width
      // case 2: r1.x <= r2.x < r1.x + r1.width
      if((r2.x<=r1.x && r1.x<r2.x+r2.width) ||
	 (r1.x<=r2.x && r2.x<r1.x+r1.width)){
        if(s != l1){
          // if we didn't have a parent already, just take this one
          map[l1].parent = r1.parent = r2.parent;
          s = l1;
        }else if(r1.parent != r2.parent){
          // otherwise union two parents if they are different

          // find terminal roots of each path up tree
          i = r1.parent;
          while(i != map[i].parent) i = map[i].parent;
          j = r2.parent;
          while(j != map[j].parent) j = map[j].parent;

          // union and compress paths; use smaller of two possible
          // representative indicies to preserve DAG property
          if(i < j){
	    map[j].parent = i;
            map[l1].parent = map[l2].parent = r1.parent = r2.parent = i;
          }else{
            map[i].parent = j;
            map[l1].parent = map[l2].parent = r1.parent = r2.parent = j;
          }
        }
      }
    }

    // Move to next point where values may change
    i = (r2.x + r2.width) - (r1.x + r1.width);
    if(i >= 0) r1 = map[++l1];
    if(i <= 0) r2 = map[++l2];
  }

  // Now we need to compress all parent paths
  for(i=0; i<num; i++){
    j = map[i].parent;
    map[i].parent = map[j].parent;
  }
}


// sum of integers over range [x,x+w)
inline int range_sum(int x, int w)
{
	return(w*(2*x + w-1) / 2);
}



void SEGMENTATION::ExtractRegions()
// Takes the list of runs and formats them into a region table,
// gathering the various statistics along the way.  num is the number
// of runs in the rmap array, and the number of unique regions in
// reg[] (bounded by max_reg) is returned.  Implemented as a single
// pass over the array of runs.
{
  int b, i, n, a;
  int num = this->run_c;
  run *rmap = this->rle;
  region *reg = this->regions;
  run r;

  n = 0;


  for(i=0; i<num; i++){
    if( rmap[i].color != NOCOLOR ){
      r = rmap[i];
      if(r.parent == i){
        // Add new region if this run is a root (i.e. self parented)
        rmap[i].parent = b = n;  // renumber to point to region id
        reg[b].color = r.color;
        reg[b].area = r.width;
        reg[b].x1 = r.x;
        reg[b].y1 = r.y;
        reg[b].x2 = r.x + r.width;
        reg[b].y2 = r.y;
        reg[b].cen_x = range_sum(r.x,r.width);
        reg[b].cen_y = r.y * r.width;
	reg[b].run_start = i;
	reg[b].iterator_id = i; // temporarily use to store last run
        n++;
        if(n >= this->max_reg) {
			printf( "Regions buffer exceeded.\n" );
			this->region_c = this->max_reg;
			return;
		}
      }else{
        // Otherwise update region stats incrementally
        b = rmap[r.parent].parent;
        rmap[i].parent = b; // update parent to identify region id
        reg[b].area += r.width;
        reg[b].x2 = std::max(r.x + r.width,reg[b].x2);
        reg[b].x1 = std::min((int)r.x,reg[b].x1);
        reg[b].y2 = r.y; // last set by lowest run
        reg[b].cen_x += range_sum(r.x,r.width);
        reg[b].cen_y += r.y * r.width;
	// set previous run to point to this one as next
	rmap[reg[b].iterator_id].next = i;
	reg[b].iterator_id = i;
      }
    }
  }

  // calculate centroids from stored sums
  for(i=0; i<n; i++){
    a = reg[i].area;
    reg[i].cen_x = (float)reg[i].cen_x / a;
    reg[i].cen_y = (float)reg[i].cen_y / a;
    rmap[reg[i].iterator_id].next = 0; // -1;
    reg[i].iterator_id = 0;
    reg[i].x2--; // change to inclusive range
  }

  this->region_c = n;
}


void SEGMENTATION::SeparateRegions()
// Splits the various regions in the region table a separate list for
// each color.  The lists are threaded through the table using the
// region's 'next' field.  Returns the maximal area of the regions,
// which can be used later to speed up sorting.
{
  region *p = NULL;
  int i;
  int c;
  int area;
  int num = this->region_c;
  region *reg = this->regions;
  color_class_state *color = this->colors;


  // clear out the region list head table
  for(i=0; i<COLOR_COUNT; i++) {
    color[i].list = NULL;
    color[i].num  = 0;
	color[i].min_area = 0;
	color[i].color = i;
  }

  // step over the table, adding successive
  // regions to the front of each list
  max_area = 0;
  for(i=0; i<num; i++){
    p = &reg[i];
    c = p->color;
    area = p->area;

    if(area >= color[c].min_area){
      if(area > max_area) max_area = area;
      color[c].num++;
      p->next = color[c].list;
      color[c].list = p;
    }
  }
}



void SEGMENTATION::SortRegions()
// Sorts entire region table by area, using the above
// function to sort each threaded region list.
{
  int i, p;

  // do minimal number of passes sufficient to touch all set bits
  p = 0;
  while( this->max_area != 0 ) {
    this->max_area >>= CMV_RBITS;
    p++;
  }

  // sort each list
  for(i=0; i<COLOR_COUNT; i++){
    this->colors[i].list = SortRegionListByArea(this->colors[i].list, p);
  }
}



region* SEGMENTATION::SortRegionListByArea( region *list, int passes )
// Sorts a list of regions by their area field.
// Uses a linked list based radix sort to process the list.
{
  region *tbl[CMV_RADIX]={NULL}, *p=NULL, *pn=NULL;
  int slot, shift;
  int i, j;

  // Handle trivial cases
  if(!list || !list->next) return(list);

  // Initialize table
  for(j=0; j<CMV_RADIX; j++) tbl[j] = NULL;

  for(i=0; i<passes; i++){
    // split list into buckets
    shift = CMV_RBITS * i;
    p = list;
    while(p){
      pn = p->next;
      slot = ((p->area) >> shift) & CMV_RMASK;
      p->next = tbl[slot];
      tbl[slot] = p;
      p = pn;
    }

    // integrate back into partially ordered list
    list = NULL;
    for(j=0; j<CMV_RADIX; j++){
      p = tbl[j];
      tbl[j] = NULL; // clear out table for next pass
      while(p){
        pn = p->next;
        p->next = list;
        list = p;
        p = pn;
      }
    }
  }

  return(list);
}


