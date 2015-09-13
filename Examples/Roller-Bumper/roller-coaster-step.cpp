#include <cmath>
#include <cassert>

using namespace std;

#define NRANSI

#include "nr.h"
#include "nrutil.h"

#include "roller-coaster-step.h"

/* Numerical Recipes Uses these Globals. */
float dxsav,*xp,**yp;
int kmax,kount;

/* Problem paraeters set at initialization. */
int nCars;    /* Number of cars. */
int nDims;    /* Dimension of dynamical system. (Equals 2*nCars) */
int f;        /* Oscillation frequency of track. */
float h;      /* Oscillation amplitude of track. */
float R;      /* Radius of track.*/
float r;      /* Radius of wheel.*/
float g;      /* Acceleration of gravity. */
float ff;     /* Square of f. */
float RR;     /* Square of R. */
float hh;     /* Square of h. */


/* Compute acceleration of a car based on its position (theta) and velocity (thetaDot). */
float acceleration(float theta, float thetaDot)
{
  float term1, term2, cosine, numerator, denomenator;
  cosine = cos(f*theta);
  term1 = (f*ff)*(hh)*(thetaDot*thetaDot)*sin(f*theta)*cosine;
  term2 = f*g*h*cosine;
  numerator = term1 - term2;
  denomenator = RR + ff*hh*cosine*cosine;
  return numerator / denomenator;
}




/* Compute the system derivative, given the time t and the current state y[1..nDims].
   Return the derivative in dydt[1..nDims]. */
void derivs(float t, float y[], float dydt[])
{
   int i;        /* Loop counter. */

   /* Derivatives of car positions. */
   for (i = 1; i <= nDims-2; i+=3) dydt[i]= y[i+1];

   /* Derivatives of car velocities. */
   for (i = 2; i <= nDims-1; i+=3) dydt[i]= acceleration(y[i-1],y[i]);

   /* Derivatives of wheel angles. */
   for (i = 3; i <= nDims; i+=3)
       {
	 float cosine = cos(f*y[i-2]);
	 dydt[i]= - (y[i-1]/r)*sqrt(RR+hh*ff*cosine*cosine);
       }
}

/* Initialize global variables needed by coasterStep. On entry, nCarsInit is the number of cars;
   lInit is the length of each car; fInit is the oscillation frequency of the track; hInit is the
   oscillation amplitude of the track; RInit is the radius of the track, rInit is the radius of a
   wheel, gInit is the acceleration of gravity. */
void coasterInitialize(int nCarsInit, int fInit, float hInit,
                       float RInit, float rInit, float gInit)
{

 nCars = nCarsInit;
 nDims = 3*nCars;
 f = fInit;
 h = hInit;
 R = RInit;
 r = rInit;
 g = gInit;
 ff = f*f;
 RR = R*R;
 hh = h*h;

 /* printf("nCars:  %3d \n", nCars);
 printf("nDims:  %3d \n", nDims);
 printf("f:      %10.4f \n", f);
 printf("h:      %10.4f \n", h);
 printf("R:      %10.4f \n", R);
 printf("r:      %10.4f \n", r);
 printf("g:      %10.4f \n", g);
 printf("ff:      %10.4f \n", ff);
 printf("RR:      %10.4f \n", RR);
 printf("hh:      %10.4f \n", hh);  */
}

void coasterStep(float theta[], float thetaDot[], float phi[], float deltaT)
{

  int i,nbad,nok;

  float eps=1.0e-4,h1=0.1,hmin=0.0,*ystart;

  /* The vector ystart holds the state of the dynamical system. */
  ystart=vector(1,nDims);

  xp=vector(1,400);
  yp=matrix(1,nDims,1,400);

  /* Initialize the system state. The array ystart holds alternating
     position and velocity values. */
  for (i=0; i<nCars; i++)
    {
      ystart[3*i+1]=theta[i];
      ystart[3*i+2]=thetaDot[i];
      ystart[3*i+3]=phi[i];
    }
  kmax=100;
  dxsav=(deltaT)/20.0;

  /* Use Runge-Kutta to integrate the system from 0.0 to deltaT starting at ystart,
     using derivs to compute the system derivative. */
  odeint(ystart,nDims,0.0,deltaT,eps,h1,hmin,&nok,&nbad,derivs,rkqs);

  /* Return the new system state in the arrays theta and thetaDot. */
  for (i=0; i<nCars; i++)
    {
      theta[i] = ystart[3*i+1];
      thetaDot[i] = ystart[3*i+2];
      phi[i] = ystart[3*i+3];
    }
  free_matrix(yp,1,nDims,1,400);
  free_vector(xp,1,400);
  free_vector(ystart,1,nDims);

}

#undef NRANSI


