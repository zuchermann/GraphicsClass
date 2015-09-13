#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>

using namespace std;

#define LOG05  -0.693147180559945  /* log(0.5) */
#define FLOOR(x) ((int)(x) - ((x) < 0 && (x) != (int)(x)))
#define CEIL(x) ((int)(x) + ((x) > 0 && (x) != (int)(x)))
#define CLAMP(x,a,b) ((x) =< (a) ? (a) : ((x) >= (b) ? (b) : (x)))
#define LERP(t,x0,x1)  ((x0) + (t)*((x1)-(x0)))
#define PULSE(a,b,x) (step((a),(x)) - step((b),(x)))
#define boxstep(a,b,x) clamp(((x)-(a))/((b)-(a)),0,1)
#define SMOOTHSTEP(x)  ((x)*(x)*(3 - 2*(x)))
#define TABSIZE          256
#define TABMASK          (TABSIZE-1)
#define PERM(x)          perm[(x)&TABMASK]
#define INDEX(ix,iy,iz)  PERM((ix)+PERM((iy)+PERM(iz)))
//#define RANDMASK  0x7fffffff
//#define RANDNBR   ((rand() & RANDMASK)/(double) RANDMASK)
#define RANDNBR (rand()/(double) RAND_MAX)
#define M_PI 3.1415926535897932384626433832795

/* Coefficients of basis matrix. */
#define CR00     -0.5
#define CR01      1.5
#define CR02     -1.5
#define CR03      0.5
#define CR10      1.0
#define CR11     -2.5
#define CR12      2.0
#define CR13     -0.5
#define CR20     -0.5
#define CR21      0.0
#define CR22      0.5
#define CR23      0.0
#define CR30      0.0
#define CR31      1.0
#define CR32      0.0
#define CR33      0.0


static double glattice(int ix, int iy, int iz, double fx, double fy, double fz);

static double vlattice(int ix, int iy, int iz);

unsigned char perm[TABSIZE] = {
        225,155,210,108,175,199,221,144,203,116, 70,213, 69,158, 33,252,
          5, 82,173,133,222,139,174, 27,  9, 71, 90,246, 75,130, 91,191,
        169,138,  2,151,194,235, 81,  7, 25,113,228,159,205,253,134,142,
        248, 65,224,217, 22,121,229, 63, 89,103, 96,104,156, 17,201,129,
         36,  8,165,110,237,117,231, 56,132,211,152, 20,181,111,239,218,
        170,163, 51,172,157, 47, 80,212,176,250, 87, 49, 99,242,136,189,
        162,115, 44, 43,124, 94,150, 16,141,247, 32, 10,198,223,255, 72,
         53,131, 84, 57,220,197, 58, 50,208, 11,241, 28,  3,192, 62,202,
         18,215,153, 24, 76, 41, 15,179, 39, 46, 55,  6,128,167, 23,188,
        106, 34,187,140,164, 73,112,182,244,195,227, 13, 35, 77,196,185,
         26,200,226,119, 31,123,168,125,249, 68,183,230,177,135,160,180,
         12,  1,243,148,102,166, 38,238,251, 37,240,126, 64, 74,161, 40,
        184,149,171,178,101, 66, 29, 59,146, 61,254,107, 42, 86,154,  4,
        236,232,120, 21,233,209, 45, 98,193,114, 78, 19,206, 14,118,127,
         48, 79,147, 85, 30,207,219, 54, 88,234,190,122, 95, 67,143,109,
        137,214,145, 93, 92,100,245,  0,216,186, 60, 83,105, 97,204, 52
};

double clamp(double x, double a, double b)
{
    return (x < a ? a : (x > b ? b : x));
}

double spline(double x, int nknots, double *knot)
{
    int span;
    int nspans = nknots - 3;
    double c0, c1, c2, c3;	/* coefficients of the cubic.*/

    if (nspans < 1) {  /* illegal */
        fprintf(stderr, "Spline has too few knots.\n");
        return 0;
    }

    /* Find the appropriate 4-point span of the spline. */
    x = clamp(x, 0, 1) * nspans;
    span = (int) x;
    if (span >= nknots - 3)
        span = nknots - 3;
    x -= span;
    knot += span;

    /* Evaluate the span cubic at x using Horner's rule. */
    c3 = CR00*knot[0] + CR01*knot[1]
       + CR02*knot[2] + CR03*knot[3];
    c2 = CR10*knot[0] + CR11*knot[1]
       + CR12*knot[2] + CR13*knot[3];
    c1 = CR20*knot[0] + CR21*knot[1]
       + CR22*knot[2] + CR23*knot[3];
    c0 = CR30*knot[0] + CR31*knot[1]
       + CR32*knot[2] + CR33*knot[3];

    return ((c3*x + c2)*x + c1)*x + c0;
}

double gradientTab[TABSIZE*3];
double valueTab[TABSIZE];

double bias(double b, double x)
{
    return pow(x, log(b)/LOG05);
}

double gain(double g, double x)
{
    if (x < 0.5) return bias(1-g, 2*x)/2;
    else return 1 - bias(1-g, 2 - 2*x)/2;
}

double gnoise(double x, double y, double z)
{
    int ix, iy, iz;
    double fx0, fx1, fy0, fy1, fz0, fz1;
    double wx, wy, wz;
    double vx0, vx1, vy0, vy1, vz0, vz1;

    ix = FLOOR(x);
    fx0 = x - ix;
    fx1 = fx0 - 1;
    wx = SMOOTHSTEP(fx0);

    iy = FLOOR(y);
    fy0 = y - iy;
    fy1 = fy0 - 1;
    wy = SMOOTHSTEP(fy0);

    iz = FLOOR(z);
    fz0 = z - iz;
    fz1 = fz0 - 1;
    wz = SMOOTHSTEP(fz0);

    vx0 = glattice(ix,iy,iz,fx0,fy0,fz0);
    vx1 = glattice(ix+1,iy,iz,fx1,fy0,fz0);
    vy0 = LERP(wx, vx0, vx1);
    vx0 = glattice(ix,iy+1,iz,fx0,fy1,fz0);
    vx1 = glattice(ix+1,iy+1,iz,fx1,fy1,fz0);
    vy1 = LERP(wx, vx0, vx1);
    vz0 = LERP(wy, vy0, vy1);

    vx0 = glattice(ix,iy,iz+1,fx0,fy0,fz1);
    vx1 = glattice(ix+1,iy,iz+1,fx1,fy0,fz1);
    vy0 = LERP(wx, vx0, vx1);
    vx0 = glattice(ix,iy+1,iz+1,fx0,fy1,fz1);
    vx1 = glattice(ix+1,iy+1,iz+1,fx1,fy1,fz1);
    vy1 = LERP(wx, vx0, vx1);
    vz1 = LERP(wy, vy0, vy1);

    return LERP(wz, vz0, vz1);
}

void gradientTabInit()
{
    double *table = gradientTab;
    double z, r, theta;
    int i;

    srand((unsigned int)clock());
    for(i = 0; i < TABSIZE; i++) {
        z = 1. - 2.*RANDNBR;
        /* r is radius of x,y circle */
        r = sqrt(1 - z*z);
        /* theta is angle in (x,y) */
        theta = 2 * M_PI * RANDNBR;
        *table++ = r * cos(theta);
        *table++ = r * sin(theta);
        *table++ = z;
    }
}

double glattice(int ix, int iy, int iz, double fx, double fy, double fz)
{
    double *g = &gradientTab[INDEX(ix,iy,iz)*3];
    return g[0]*fx + g[1]*fy + g[2]*fz;
}

double vnoise(double x, double y, double z)
{
    int ix, iy, iz;
    int i, j, k;
    double fx, fy, fz;
    double xknots[4], yknots[4], zknots[4];

    ix = FLOOR(x);
    fx = x - ix;

    iy = FLOOR(y);
    fy = y - iy;

    iz = FLOOR(z);
    fz = z - iz;

    for (k = -1; k <= 2; k++) {
        for (j = -1; j <= 2; j++) {
            for (i = -1; i <= 2; i++)
                xknots[i+1] = vlattice(ix+i,iy+j,iz+k);
            yknots[j+1] = spline(fx, 4, xknots);
        }
        zknots[k+1] = spline(fy, 4, yknots);
    }
    return spline(fz, 4, zknots);
}

void valueTabInit()
{
    double *table = valueTab;
    int i;
    srand((unsigned int)clock());
    for(i = 0; i < TABSIZE; i++)
        *table++ = 1. - 2.*RANDNBR;
}

double vlattice(int ix, int iy, int iz)
{
    return valueTab[INDEX(ix,iy,iz)];
}
