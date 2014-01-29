#ifndef TRILATERAL2003
#define TRILATERAL2003

#include <tgmath.h>
#include <math.h>
#include <cstddef>
#include <ostream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include "MyConstants.h"
#include "GrafixProject.h"
#include "GPGeneralStaticLibrary.h"
#include "GrafixSettingsLoader.h"


#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QProgressDialog>

#include "armadillo"

/*
 *
Slightly edited by J.Alvarez from original.

This C++ source code fragment will trilateral filter a 2D scalar image,
as described in:
==============================================================================
    "Trilateral Filter for HDR Images and 3D Meshes"
    Prasun Choudhury and Jack Tumblin,
	Proc. Eurographics Symposium on Rendering,
	Per. H. Christensen and Daniel Cohen eds., pp. 186-196, 2003.
==============================================================================
  A few items in the code may need some explanation:
  
    --'Raw2D' is a floating-point 2D image class that holds a rectangular grid
	of pixels; images are re-sizeable, and indexed by 0 <= (x,y) < (xmax,ymax).
	The pixel datatype is set by '#DEFINE PIXTYPE float'.

	--'Raw3D' holds a 1D array of Raw2D objects, indexed by (x,y,z). 

    --Helper functions for Raw2D objects:
		PIXTYPE get(int x,int y);	--returns the PIXTYPE pixel value at (x,y)
        void put(int x,int y,PIXTYPE val)
									--write 'val' at pixel location (x,y)
            	sizer(arg)          -- set array size to match 'arg'
            	wipecopy(src)		-- copy contents of 'src', resize as needed.

     --Trilateral Filter() is a Raw2D member function. Its inputs are:
		srcImg -- input image (luminance/intensity)
		sigmaC -- variance of the domain  for the gradient and 
					the detail filter (Section 3.3)

    --Quick overview:
		1) Compute the X and Y gradients of the input image 
			and store it in xGradient and yGradient.

		2) Compute the min-gradient stack and max-gradient stack from the 
			gradients computed in step 1; store result in minGradientStack 
			and maxGradientStack respectively,

		3) Bilaterally filter the gradients (xGradient and yGradient) of the 
			input image; store results in xSmoothGradient and ySmoothGradient.
			(equations 4 and 5, Section 3.1)

		4) Compute the adaptive neighborhood 'fTheta' from the min and max 
		stack of image gradients. (equation 10, Section 3.2)

		5) Tilt the filter kernel based on bilaterally smoothed image gradients
		(xSmoothGradient snd ySmoothGradient) and filter the detail signal. 
		Then we add filtered detail to the original signal 
		(equations 6, 7, 8, 9; Section 3.1).

If you still have questions, 
	please send an e-mail to: p-choudhury@northwestern.edu.

    Prasun Choudhury and Jack Tumblin  7/15/2003
*/


#define PIXTYPE double
#define M_EXP 2.7182818284590452353602874713527

//=====================
// Forward Declarations	
//=====================
class Raw2D;
class Raw3D; 

/**
	A 2D array of pixels, consisting of an array of PIXTYPE objects. Note
	that the Raw1D objects are not required to be the same size, though
	the sizer() function will create them as all the same size.
**/

class Raw2D
{
private:   			//-----------------DATA----------------- 
    	int xsize;		// # of pixels per scanline,
    	int ysize;		// # of scanlines in this Raw2D.
    	PIXTYPE *y;		// 1D array of PIXTYPE that are accessed as a 2D array.

public:				//---------------init fcns-------------
    	Raw2D(void);		// constructor for 'empty' Raw2Ds
    	~Raw2D(void);		// destructor; releases memory
   	void sizer(int ixsize, int iysize);		// get mem for rectangle of pixels
   	void sizer(Raw2D* src);					// get same amt. of mem as 'src'
	int getXsize(void) {return xsize;};		// get # pixels per scanline
	int getYsize(void) {return ysize;};		// get # of scanlines.
    bool wipecopy(Raw2D* src);			// copy, even with size mismatch
   	void put(int ix, int iy, PIXTYPE val) {	// write 'val' at location ix,iy.
	y[ix + xsize*iy] = val; 
	};
   	PIXTYPE get(int ix, int iy) {	// read the value at ix,iy.
		return(y[ix + xsize*iy]); 
	};
	PIXTYPE getXY(int ixy){		// read value at 1D address ixy
		return y[ixy];
	};
	void putXY(int ixy,PIXTYPE val){// write value at 1D address ixy
		y[ixy] = val;
	};
				//---------------Trilateral Filter fcns-------------

	//Trilateral filter consisting of gradient filter, adaptive neighborhood
	//computation and detail filter
    static void TrilateralFilter(Raw2D* srcImg, Raw2D* pDestImg, PIXTYPE sigmaC, double sigmaR_custom);

	//Computes X and Y gradients of the input image
   	void ComputeGradients(Raw2D* pX, Raw2D* pY); 

	//Bilaterally filters  gradients pX and pY 
   	void BilateralGradientFilter(Raw2D* pX, Raw2D* pY, Raw2D* pSmoothX, 
		Raw2D* pSmoothY, PIXTYPE sigmaC, PIXTYPE sigmaS, int filterSize); 

	//Builds the stack of min-max image gradients; returns the range variance
   	PIXTYPE buildMinMaxImageStack(Raw2D* pX, Raw2D* pY, Raw3D* pMinStack,
		Raw3D* pMaxStack , int levelMax, PIXTYPE beta); 

	//Finds the adaptive neighborhood size (stack level) 
	//from the min-max gradient stack
   	void findAdaptiveRegion(Raw3D* pMinStack, Raw3D* pMaxStack, PIXTYPE R, int levelMax); 
						
	//Filters the detail signal and computes the final output image	
   	void DetailBilateralFilter(Raw2D* srcImg, Raw2D* pSmoothX, Raw2D* pSmoothY, 
		Raw2D* fTheta, PIXTYPE sigmaCTheta, PIXTYPE sigmaRTheta); 
    	
};



/** 
	A 3D array of pixels, consisting of a 1D array of Raw2D objects. 
	Use this class for images, with one Raw2D object for each 
	a 'field' of that image, such as R,G,B,A, etc. 
**/

class Raw3D
{
public:
    	Raw2D *z;	// dynam. allocated space for a set of Raw2D objects.
    	int zsize;	// # of Raw2D objects stored.
																		
public:							
    	Raw3D(void);	// 'empty' Raw3D constructor.
    	~Raw3D(void);	// destructor.
    	void sizer(int ixsize, int iysize, int izsize); // reserve memory
    	void sizer(Raw3D* src);			// get same amt. of mem as 'src
	int getZsize(void) {				// return # of Raw2D's we hold;
		return(zsize); 
	};
	int getYsize() {					// # of Raw1D's in zval-th Raw2D;
		return(z[0].getYsize()); 
	};
	int getXsize(){						// # of pixels on yval,zval-th line
		return(z[0].getXsize()); 
	};
    	PIXTYPE get(int ix, int iy, int iz) {
    		return(z[iz].get(ix,iy));	// write 'val' at location ix,iy,iz. 
    	};
    	void put(int ix, int iy, int iz, PIXTYPE val) { 
	    	z[iz].put(ix,iy,val);		//write 'val' at location ix,iy,iz.
    	};
	void wipecopy(Raw3D& src);			// copy, resize as needed.
};

#endif
