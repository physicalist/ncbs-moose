
/*
 * Examples of gnuplot_i.c usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gnuplot_i.h"

#define SLEEP_LGTH  5
#define NPOINTS     15000

int main(int argc, char *argv[]) 
{
    gnuplot_ctrl	*h1;

    double              x[NPOINTS] ;
    double              y[NPOINTS] ;
    int                 i ;
    int			iCount;

    /*
     * Initialize the gnuplot handle
     */
    printf("*** example of gnuplot control through C ***\n") ;
    h1 = gnuplot_init() ;
    gnuplot_cmd(h1, "plot sin(x)");
    sleep(10);

    gnuplot_resetplot(h1) ;

    gnuplot_setstyle(h1, "points") ;
    /*printf("\n\n") ;
    printf("*** user-defined lists of doubles\n") ;
    for (i=0 ; i<5000; i++) {
        x[i] = sqrt(i) ;
    }
    gnuplot_plot_x(h1, x, 5000, "user-defined doubles") ;
    //sleep(SLEEP_LGTH) ;

    
    for (i=5000 ; i<NPOINTS; i++) {
        x[i] = sqrt(i) ;
    }
    gnuplot_plot_x(h1, x, NPOINTS, "user-defined doubles") ;
    sleep(SLEEP_LGTH) ;*/

  double lXValue = 0;
  for(iCount =0; iCount < 5000; iCount++)
  {

    for (i=0 ; i<NPOINTS ; i++) {
	lXValue++;
        x[i] = (double)lXValue;
        y[i] = (double)i * (double)i ;
    }
    gnuplot_plot_xy_file(h1, x, y, NPOINTS, "user-defined points") ;
  }


    printf("\n\n") ;
    printf("*** end of gnuplot example\n") ;
    gnuplot_close(h1) ;
    return 0 ;
}

