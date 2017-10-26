//------------------------------------------------------------------------
// galfit_min.cpp: Members functions for minimization of the Galfit class.
//------------------------------------------------------------------------

/*-----------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the
 Free Software Foundation; either version 2 of the License, or (at your
 option) any later version.

 BBarolo is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License
 along with BBarolo; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA

 Correspondence concerning BBarolo may be directed to:
    Internet email: enrico.diteodoro@unibo.it
-----------------------------------------------------------------------*/

#include <iostream>
#include <cfloat>
#include <cmath>
#include <Arrays/cube.hh>
#include <Arrays/image.hh>
#include <Map/voxel.hh>
#include <Utilities/galfit.hh>
#include <Utilities/utils.hh>
#include <Utilities/galmod.hh>
#include <Utilities/conv2D.hh>

using namespace PixelInfo;

#define VROT  0
#define VDISP 1
#define DENS  2
#define Z0    3
#define INC   4
#define PA    5
#define XPOS  6
#define YPOS  7
#define VSYS  8
#define VRAD  9
#define MAXPAR 10

namespace Model {

template <class T>
bool Galfit<T>::minimize(Rings<T> *dring, T &minimum, T *pmin) {
	
	/// This function uses the Downhill Simplex Method 
	/// in multidimensions due to Nelder and Mead.
	
    const int NMAX=2000;
	const double TINY=1.0e-10;
	
    int ndim=nfree;
    if (global) ndim=nfree*dring->nr;

	int mpts=ndim+1;
	T **p = allocate_2D<T>(mpts,ndim);
	
    T *point = new T[ndim];
	T *dels  = new T[ndim];

    /*
    int k=0;
    if (mpar[VROT])  point[k++]=dring->vrot.front();
    if (mpar[VDISP]) point[k++]=dring->vdisp.front();
    if (mpar[DENS])  point[k++]=dring->dens.front();
    if (mpar[Z0])    point[k++]=dring->z0.front();
    if (mpar[INC])   point[k++]=dring->inc.front();
    if (mpar[PA])    point[k++]=dring->phi.front();
    if (mpar[XPOS])  point[k++]=dring->xpos.front();
    if (mpar[YPOS])  point[k++]=dring->ypos.front();
    if (mpar[VSYS])  point[k++]=dring->vsys.front();
    if (mpar[VRAD])  point[k++]=dring->vrad.front();

    /// Determine the initial simplex.
	for (int i=0; i<ndim; i++) {
		dels[i]  = 0.2*point[i]; 
		point[i] = point[i]-0.1*point[i];					///-<<<<<<<<<<<< Totalmente arbitrario. pensaci su.
	}
    */

    /// Determine the initial simplex (new way).
//    if (global) {
//        int n=dring->nr, k=0;
//        if (mpar[VROT])  for (int j=0;j<n;j++) {dels[k]=30.; point[k++]=dring->vrot[j];}
//        if (mpar[VDISP]) for (int j=0;j<n;j++) {dels[k]=10.; point[k++]=dring->vdisp[j];}
//        if (mpar[DENS])  for (int j=0;j<n;j++) {dels[k]=0.2*dring->dens.front(); point[k++]=dring->dens[j];}
//        if (mpar[Z0])    for (int j=0;j<n;j++) {dels[k]=0.2*dring->z0.front(); point[k++]=dring->z0[j];}
//        if (mpar[INC])   for (int j=0;j<n;j++) {dels[k]=5.; point[k++]=dring->vexp[j];}
//        if (mpar[PA])    for (int j=0;j<n;j++) {dels[k]=10.; point[k++]=dring->phi[j];}
//        if (mpar[XPOS])  for (int j=0;j<n;j++) {dels[k]=(maxs[XPOS]-mins[XPOS])/4.; point[k++]=dring->xpos[j];}
//        if (mpar[YPOS])  for (int j=0;j<n;j++) {dels[k]=(maxs[YPOS]-mins[YPOS])/4.; point[k++]=dring->ypos[j];}
//        if (mpar[VSYS])  for (int j=0;j<n;j++) {dels[k]=(maxs[VSYS]-mins[VSYS])/4.; point[k++]=dring->vsys[j];}
    //        if (mpar[VRAD])  for (int j=0;j<n;j++) {dels[k]=(maxs[VRAD]-mins[VRAD])/4.; point[k++]=dring->vrad[j];}
//    }
//    else {
//        int k=0;
//        if (mpar[VROT])  {dels[k]=30.; point[k++]=dring->vrot.front();}
//        if (mpar[VDISP]) {dels[k]=10.; point[k++]= dring->vdisp.front();}
//        if (mpar[DENS])  {dels[k]=0.2*dring->dens.front(); point[k++]= dring->dens.front();}
//        if (mpar[Z0])    {dels[k]=0.2*dring->z0.front(); point[k++]= dring->z0.front();}
//        if (mpar[INC])   {dels[k]=5.; point[k++]= dring->inc.front();}
//        if (mpar[PA])    {dels[k]=10.;point[k++]= dring->phi.front();}
//        if (mpar[XPOS])  {dels[k]=(maxs[XPOS]-mins[XPOS])/4.;point[k++]= dring->xpos.front();}
//        if (mpar[YPOS])  {dels[k]=(maxs[YPOS]-mins[YPOS])/4.;point[k++]= dring->ypos.front();}
//        if (mpar[VSYS])  {dels[k]=(maxs[VSYS]-mins[VSYS])/4.;point[k++]= dring->vsys.front();}
//        if (mpar[VRAD])  {dels[k]=(maxs[VRAD]-mins[VRAD])/4.;point[k++]= dring->vrad.front();}
//    }
    int n=1, k=0;
    if (global) n=dring->nr;
    if (mpar[VROT]) {
        for (int j=0;j<n;j++) {
            if (dring->vrot[j]>40) dels[k]=30.;
            else dels[k]=5;
            point[k++]=dring->vrot[j];
        }
    }
    if (mpar[VDISP]) {
        for (int j=0;j<n;j++) {
            if (dring->vdisp[j]>10+mins[VDISP]) dels[k]=10.;
            else dels[k]=mins[VDISP];
            point[k++]=dring->vdisp[j];
        }
    }
    if (mpar[DENS])  for (int j=0;j<n;j++) {dels[k]=0.2*dring->dens.front(); point[k++]=dring->dens[j];}
    if (mpar[Z0])    for (int j=0;j<n;j++) {dels[k]=0.2*dring->z0.front(); point[k++]=dring->z0[j];}
    if (mpar[INC])   for (int j=0;j<n;j++) {dels[k]=5.; point[k++]=dring->inc[j];}
    if (mpar[PA])    for (int j=0;j<n;j++) {dels[k]=10.; point[k++]=dring->phi[j];}
    if (mpar[XPOS])  for (int j=0;j<n;j++) {dels[k]=(maxs[XPOS]-mins[XPOS])/4.; point[k++]=dring->xpos[j];}
    if (mpar[YPOS])  for (int j=0;j<n;j++) {dels[k]=(maxs[YPOS]-mins[YPOS])/4.; point[k++]=dring->ypos[j];}
    if (mpar[VSYS])  for (int j=0;j<n;j++) {dels[k]=(maxs[VSYS]-mins[VSYS])/4.; point[k++]=dring->vsys[j];}
    if (mpar[VRAD])  for (int j=0;j<n;j++) {dels[k]=10.; point[k++]=dring->vrad[j];}

    // Build the initial matrix.
	for (int i=0; i<mpts; i++) {
			for (int j=0; j<ndim; j++) p[i][j]=point[j];
		if (i!=0) p[i][i-1] += dels[i-1];
	}

	delete [] point;
	delete [] dels;
	
	
	T psum[ndim], x[ndim];
	T *y = new T[mpts];
	
	for (int i=0; i<mpts; i++) {
        for (int j=0; j<ndim; j++) x[j]=p[i][j];
        y[i]=func3D(dring,x);
	}
	
	int nfunc=0;
	for (int j=0; j<ndim; j++) {
		T sum=0.0;
		for (int i=0; i<mpts; i++) sum += p[i][j];
		psum[j]=sum;
	}
	
	// Main cycle begins here.
	for (;;) {
		int ihi, inhi;
		int ilo=0;
		// First determine the highest point (worst) and the 
		// lowest (best).
		ihi = y[0]>y[1] ? (inhi=1,0) : (inhi=0,1);
		for (int i=0; i<mpts; i++) {
			if (y[i]<=y[ilo]) ilo=i;
			if (y[i]>y[ihi]) {
				inhi=ihi;
				ihi=i;
			} 
			else if (y[i]>y[inhi] && i!=ihi) inhi=i;
		}
		
		// Compute the fractional range from highest to lowest.
		double rtol=2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo])+TINY);
		
		// If it is satisfactory, put best point and value in slot 0;
		if (rtol<tol) { 	
			std::swap(y[0],y[ilo]);
			for (int i=0; i<ndim; i++) {
				std::swap(p[0][i],p[ilo][i]);
				pmin[i]=p[0][i];
			}
			minimum=y[0];
			deallocate_2D(p,ndim+1);
			delete [] y;
			return true;
		}
		
		if (nfunc>=NMAX) {
            std::cout <<"\n ========================== 3DFIT ERROR ==========================\n"
					  << " Can not achieve convergence in this ring. I'll keep going on, but \n" 
					  << " parameters for this ring are wrong! Please, try to change initial \n"
					  << " conditions and/or the function to minimize.\n"
					  << " =================================================================\n\n";
			deallocate_2D(p,ndim+1);
			delete [] y;
			return false;
			//std::terminate();
		}
		nfunc += 2;
		
		// Try a new iteration. Try extrapolate by a factor -1 through
		// the simplex face across from the high point, i.e. reflect the
		// simplex from the high point.
        T ytry=mtry(dring, p,y,psum,ihi,-1.0);
		if (ytry<=y[ilo]) {
			// If it gives a value better than the best point, try an 
			// additional extrapolation by a factor 2.
            ytry=mtry(dring, p,y,psum,ihi,2.0);
		}
		else if (ytry>=y[inhi]) {
			// Otherwise, if the reflected point is worse than the second
			// highest, look for an intermediate lower point, i.e. do a 
			// on dimensional contraction.
			T ysave=y[ihi];
            ytry=mtry(dring, p,y,psum,ihi,0.5);
			if (ytry>=ysave) { 		
				// Can't seem to get rid of that high point. Better contract
				// around the lowest (best) point.
				for (int i=0; i<mpts; i++) {
					if (i!=ilo) {
						for (int j=0; j<ndim; j++)
							p[i][j]=psum[j]=0.5*(p[i][j]+p[ilo][j]);
                        y[i]=func3D(dring,psum);
					}
				}
				
				nfunc += ndim; 	
				
				for (int j=0;j<ndim;j++) {
					T sum=0.0;
					for (int i=0; i<mpts; i++) sum += p[i][j];
					psum[j]=sum;
				}
			}
		} 
		else --nfunc; 
	} 
	
}	
template bool Galfit<float>::minimize(Rings<float>*,float&,float*);
template bool Galfit<double>::minimize(Rings<double>*,double&,double*);
	
	
template <class T>
T Galfit<T>::mtry(Rings<T> *dring, T **p, T *y, T *psum, const int ihi, const double fac) {
	
	// Extrapolates by a factor fac through the face of the simplex across from
	// the high point, tries it, and replaces the high point if the new point is better.
	
	int ndim = nfree;
	T ptry[ndim];
	double fac1=(1.0-fac)/ndim;
	double fac2=fac1-fac;
	
	for (int j=0; j<ndim; j++)
		ptry[j]=psum[j]*fac1-p[ihi][j]*fac2;
	
	// Evaluate the function at the trial point.
    T ytry=func3D(dring, ptry);
	if (ytry<y[ihi]) { 	
		// If it is better than the highest, then replace the highest.			 
		y[ihi]=ytry;
		for (int j=0; j<ndim; j++) {
			psum[j] += ptry[j]-p[ihi][j];
			p[ihi][j]=ptry[j];
		}
	}
	return ytry;
}
template float Galfit<float>::mtry(Rings<float>*,float**,float*,float*,const int,const double);
template double Galfit<double>::mtry(Rings<double>*,double**,double*,double*,const int,const double);


template <class T>
T Galfit<T>::func3D(Rings<T> *dring, T *zpar) {


//    T vrot  = dring->vrot.front();
//    T vdisp = dring->vdisp.front();
//    T dens  = dring->dens.front();
//    T z0    = dring->z0.front();
//    T inc   = dring->inc.front();
//    T phi   = dring->phi.front();
//    T xpos  = dring->xpos.front();
//    T ypos  = dring->ypos.front();
//    T vsys  = dring->vsys.front();
//    T vrad  = dring->vrad.front();
    

    int n=1, np=0;
    if (global) {n=dring->nr; w_r=0;}
    T vrot[n],vdisp[n],dens[n],z0[n],inc[n],phi[n],xpos[n],ypos[n],vsys[n],vrad[n];

    for (int j=0; j<n; j++) {
      vrot[j]  = dring->vrot[j];
      vdisp[j] = dring->vdisp[j];
      dens[j]  = dring->dens[j];
      z0[j]    = dring->z0[j];
      inc[j]   = dring->inc[j];
      phi[j]   = dring->phi[j];
      xpos[j]  = dring->xpos[j];
      ypos[j]  = dring->ypos[j];
      vsys[j]  = dring->vsys[j];
      vrad[j]  = dring->vrad[j];
    }

    for (int i=0; i<MAXPAR; i++) {
        if (mpar[i]) {
            switch(i) {
            case VROT:
                for (int j=0; j<n; j++) {
                    vrot[j] = (zpar[np]<mins[VROT] || zpar[np]>maxs[VROT]) ? inr->vrot[w_r] : zpar[np];
                    zpar[np++] = vrot[j];
                }
                break;

            case VDISP:
                for (int j=0; j<n; j++) {
                    vdisp[j] = (zpar[np]<mins[VDISP] || zpar[np]>maxs[VDISP]) ? inr->vdisp[w_r]+0.1*mins[VDISP] : zpar[np];
                    //vdisp = mins[VDISP]+0.1*mins[VDISP];
                    //vdisp = outr->vdisp[w_r-1];
                    zpar[np++] = vdisp[j];
                }
                break;

            case DENS:
                for (int j=0; j<n; j++) dens[j] = zpar[np++];
                break;

            case Z0:
                for (int j=0; j<n; j++) {
                    z0[j] = (zpar[np]<mins[Z0] || zpar[np]>maxs[Z0]) ? inr->z0[w_r] : zpar[np];
                    zpar[np++] = z0[j];
                }
                break;

            case INC:
                for (int j=0; j<n; j++) {
                    inc[j] = (zpar[np]<mins[INC] || zpar[np]>maxs[INC]) ? inr->inc[w_r] : zpar[np];
                    zpar[np++] = inc[j];
                }
                break;

            case PA:
                for (int j=0; j<n; j++) {
                    phi[j] = (zpar[np]<mins[PA] || zpar[np]>maxs[PA]) ?  inr->phi[w_r] : zpar[np];
                    zpar[np++] = phi[j];
                }
                break;

            case XPOS:
                for (int j=0; j<n; j++) {
                    xpos[j] = (zpar[np]<mins[XPOS] || zpar[np]>maxs[XPOS]) ? inr->xpos[w_r] : zpar[np];
                    zpar[np++] = xpos[j];
                }
                break;

            case YPOS:
                for (int j=0; j<n; j++) {
                    ypos[j] = (zpar[np]<mins[YPOS] || zpar[np]>maxs[YPOS]) ? inr->ypos[w_r] : zpar[np];
                    zpar[np++] = ypos[j];
                }
                break;

            case VSYS:
                for (int j=0; j<n; j++) {
                    vsys[j] = (zpar[np]<mins[VSYS] || zpar[np]>maxs[VSYS]) ? inr->vsys[w_r] : zpar[np];
                    zpar[np] = vsys[j];
                }
                break;
            
            case VRAD:
                for (int j=0; j<n; j++) {
                    vrad[j] = (zpar[np]<mins[VRAD] || zpar[np]>maxs[VRAD]) ? inr->vrad[w_r] : zpar[np];
                    zpar[np] = vrad[j];
                }
                    break;

            default:
                break;
            }
        }
    }

    /*
    bool out_of_boundaries=false;
    for (int i=0; i<MAXPAR; i++) {
        if (mpar[i]) {
            switch(i) {
                case VROT:
                    vrot = zpar[npar];
                    if (vrot<mins[VROT] || vrot>maxs[VROT]) out_of_boundaries = true;
                    npar++;
                    break;

                case VDISP:
                    vdisp = zpar[npar];
                    if (vdisp<mins[VDISP] || vdisp>maxs[VDISP]) out_of_boundaries = true;
                    npar++;
                    break;

                case DENS:
                    dens = zpar[npar];
                    npar++;
                    break;

                case Z0:
                    z0 = zpar[npar];
                    if (z0<mins[Z0] || z0>maxs[Z0]) out_of_boundaries = true;
                    npar++;
                    break;

                case INC:
                    inc = zpar[npar];
                    if (inc<mins[INC] || inc>maxs[INC]) out_of_boundaries = true;
                    npar++;
                    break;

                case PA:
                    phi = zpar[npar];
                    if (phi<mins[PA] || phi>maxs[PA]) out_of_boundaries = true;
                    npar++;
                    break;

                case XPOS:
                    xpos = zpar[npar];
                    if (xpos<mins[XPOS] || xpos>maxs[XPOS]) out_of_boundaries = true;
                    npar++;
                    break;

                case YPOS:
                    ypos = zpar[npar];
                    if (ypos<mins[YPOS] || ypos>maxs[YPOS]) out_of_boundaries = true;
                    npar++;
                    break;

                case VSYS:
                    vsys = zpar[npar];
                    if (vsys<mins[VSYS] || vsys>maxs[VSYS]) out_of_boundaries = true;
                    npar++;
                    break;
                
                case VRAD:
                    vrad = zpar[npar];
                    if (vrad<mins[VRAD] || vrad>maxs[VRAD]) out_of_boundaries = true;
                    npar++;
                    break;

                    default:
                    break;
            }
        }
    }

    if (out_of_boundaries) return FLT_MAX;
*/

	/// Maximum rotation velocity inside the datacube
    //T maxvrot_r = fabs(AlltoVel<T>(in->getZphys(in->DimZ()-1),in->Head().Cunit(2), in->Head().Freq0())-vsys);
    //T maxvrot_l = fabs(AlltoVel<T>(in->getZphys(0),in->Head().Cunit(2),in->Head().Freq0())-vsys);
    //T maxvrot   = std::max(maxvrot_r*sin(inc*M_PI/180.),maxvrot_l*sin(inc*M_PI/180.))+20;
    //if (vrot>maxvrot && mpar[VROT]) vrot = zpar[VROT] = maxvrot;

    for (int i=0,j=0; i<dring->nr; i++) {
        j=i*global;
        dring->vrot[i] 	= vrot[j];
        dring->vdisp[i]	= vdisp[j];
        dring->z0[i]	= z0[j];
        dring->dens[i]	= dens[j];
        dring->inc[i]	= inc[j];
        dring->phi[i]	= phi[j];
        dring->xpos[i]	= xpos[j];
        dring->ypos[i]	= ypos[j];
        dring->vsys[i]	= vsys[j];
        dring->vrad[i]	= vrad[j];
	}

    return model(dring);

}
template float Galfit<float>::func3D(Rings<float>*,float*);
template double Galfit<double>::func3D(Rings<double>*,double*);


template <class T>
T Galfit<T>::model(Rings<T> *dring) {

    Model::Galmod<T> *mod = new Model::Galmod<T>;

	int xdis = ceil((dring->radii.back()+3*dring->z0.back())/(fabs(in->Head().Cdelt(0))*arcconv));
	int ydis = ceil((dring->radii.back()+3*dring->z0.back())/(fabs(in->Head().Cdelt(1))*arcconv));
	int xpos = ceil(dring->xpos.back());
	int ypos = ceil(dring->ypos.back());
    int blo[2] = {xpos-xdis, ypos-ydis};
    int bhi[2] = {xpos+xdis, ypos+ydis};
	if (blo[0]<0) blo[0] = 0;	
	if (blo[1]<0) blo[1] = 0;	
	if (bhi[0]>in->DimX()) bhi[0] = in->DimX();	
	if (bhi[1]>in->DimY()) bhi[1] = in->DimY();
	int bsize[2] = {bhi[0]-blo[0], bhi[1]-blo[1]};	
	
	int nv;
	if (in->pars().getNV()==-1) nv=in->DimZ();
	else nv = in->pars().getNV();
	int ltype = in->pars().getLTYPE();
	int cdens = in->pars().getCDENS();
    int cmode = in->pars().getCMODE();
	mod->input(in,bhi,blo,dring,nv,ltype,cmode,cdens);
	mod->calculate();
	
	T *modp = mod->Out()->Array();
	
	//<<<<< Convolution....
	if (in->pars().getSM()) {
		if (in->pars().getflagFFT()) Convolve_fft(modp, bsize);
		else Convolve(modp, bsize);
	}
		
	//<<<<< Normalizing & calculating the residuals....
    double minfunc = (this->*func_norm)(dring,modp,bhi,blo);
	
	delete mod;

	return minfunc; 

}
template float Galfit<float>::model(Rings<float>*);
template double Galfit<double>::model(Rings<double>*);


template <class T>
void Galfit<T>::Convolve(T *array, int *bsize) {
	
	if (cfieldAllocated) {
		int ndx = (bsize[0]+NconX-1);
		int ndy = (bsize[1]+NconY-1);
		T *beforeCON = new T[ndx*ndy];
		T *afterCON  = new T[ndx*ndy];
		for (int z=0; z<in->DimZ(); z++) {
			for (int x=0; x<ndx; x++) {
				for (int y=0; y<ndy; y++) {
					long nPix = x+y*ndx;
					int mXpos = x-(NconX-1)/2;
					int mYpos = y-(NconY-1)/2;
					long mPix = mXpos+mYpos*bsize[0]+z*bsize[0]*bsize[1];
					afterCON[nPix] = beforeCON[nPix] = 0;
					if (x>=(NconX-1)/2 && x<=(bsize[0]+(NconX-1)/2) && 
						y>=(NconY-1)/2 && y<=(bsize[1]+(NconY-1)/2)) {
						beforeCON[nPix] = array[mPix];	
					}
				}
			}
			
			for (int yc=0; yc<NconY; yc++) {
				for (int xc=0; xc<NconX; xc++) {
					T cf = cfield[NconX-xc-1+(NconY-yc-1)*NconX];
					if (cf!=0.0) {
						for (int y=0; y<bsize[1]; y++) {
							T *v1 = &beforeCON[(yc+y)*ndx+xc];
							T *v2 = &afterCON[(NconY/2+y)*ndx+NconX/2];
							for (int x=0; x<bsize[0]; x++) v2[x] = v2[x]+cf*v1[x];
						}
					}
				}
			}		
			
			for (int x=(NconX-1)/2; x<(bsize[0]+(NconX-1)/2); x++) {
				for (int y=(NconY-1)/2; y<(bsize[1]+(NconY-1)/2); y++) {
					long nPix = x+y*(bsize[0]+NconX-1);
					long mPix = (x-(NconX-1)/2)+(y-(NconY-1)/2)*bsize[0]+z*bsize[0]*bsize[1];	
					//if (IsIn(x-(NconX-1)/2,y-(NconY-1)/2,blo,dring)) 
						array[mPix] = afterCON[nPix];
					//else modp[mPix] = 0;
				}
			}	
		
		}
				
		delete [] beforeCON;
		delete [] afterCON;	
	}
	
}
template void Galfit<float>::Convolve(float*,int*);
template void Galfit<double>::Convolve(double*,int*);


template <class T>
void Galfit<T>::Convolve_fft(T *array, int *bsize) {
	
	if (cfieldAllocated) {
		long size = bsize[0]*bsize[1];
		double *beforeCON = new double[size];
		Conv2D cfft;
		init_Conv2D (cfft, LINEAR_SAME, bsize[0], bsize[1], NconX, NconY);	
		
		for (uint z=in->DimZ(); z--;) {
			T *ptr = &array[z*size];
			for (uint i=size; i--;) beforeCON[i] = ptr[i];
			convolve (cfft, beforeCON, cfield);			
			for (uint i=size; i--;) 
				ptr[i] = (cfft.dst[i]<1.E-12) ? 0. : cfft.dst[i];	//<<<< Un po' arbitrario, non mi piace.
		}
				
		clear_Conv2D(cfft);	
		delete [] beforeCON;
	}

}
template void Galfit<float>::Convolve_fft(float*,int*);
template void Galfit<double>::Convolve_fft(double*,int*);

///*
// ANELLO 2D
template <class T>
double Galfit<T>::norm_local (Rings<T> *dring, T *array, int *bhi, int *blo) {

	int bsize[2] = {bhi[0]-blo[0], bhi[1]-blo[1]};	

	if (!in->StatsDef()) in->setCubeStats();
	
	int numPix_ring=0, numBlanks=0, numPix_tot=0;
	double minfunc = 0;
    //int bweight = second ? 0 : in->pars().getBweight();
    int bweight = in->pars().getBweight();
		
	for (uint y=bsize[1]; y--;) {
		for (uint x=bsize[0]; x--;) {
			double theta;			
			if (!IsIn(x,y,blo,dring,theta)) continue;
            if (!getSide(theta)) continue;
			numPix_ring++;
			
			//< Factor for normalization.
            T modSum=0, obsSum = 0, factor=0;
			for (uint z=in->DimZ(); z--;) {
				long modPix = x+y*bsize[0]+z*bsize[0]*bsize[1];
				long obsPix = in->nPix(x+blo[0],y+blo[1],z);
				modSum += array[modPix];
				if (in->Array(obsPix)>0) obsSum += in->Array(obsPix)*mask[obsPix];
			}
			if (modSum!=0) factor = obsSum/modSum;
			else factor=0;										
			
			double costh = fabs(cos(theta*M_PI/180.));
			double wi = std::pow(costh, double(wpow));
			
			// Normalizing and residuals.
			for (uint z=in->DimZ(); z--;) {
				long modPix = x+y*bsize[0]+z*bsize[0]*bsize[1];
				long obsPix = in->nPix(x+blo[0],y+blo[1],z);
				array[modPix] *= factor;	
				T obs = in->Array(obsPix)>0 ? in->Array(obsPix) : in->stat().getSpread();  
				T mod = array[modPix];
				
                if (mask[obsPix]==0) {
                    if (mod==0) continue;
                    else numBlanks++;
				}
				
				numPix_tot++;
                minfunc += getFuncValue(obs,mod,wi,chan_noise[z]);
			}
		}
	}	
	//numPix_ring=1;
    //return std::pow((1+numBlanks/T(numPix_tot)),bweight)*minfunc/numPix_ring;
    return std::pow((1+numBlanks/T(numPix_tot)),bweight)*minfunc/((numPix_tot-numBlanks));
	
}
template double Galfit<float>::norm_local(Rings<float>*,float*,int*,int*);
template double Galfit<double>::norm_local(Rings<double>*,double*,int*,int*);
//*/

/*
//ANELLO 3D
template <class T>
double Galfit<T>::norm_local (Rings<T> *dring, T *array, int *bhi, int *blo) {

	int bsize[2] = {bhi[0]-blo[0], bhi[1]-blo[1]};	

	if (!in->StatsDef()) in->setCubeStats();
	
	std::vector<Pixel<T> > *anulus = getRingRegion(dring, bhi, blo);
	
	int numPix_ring=0, numBlanks=0, numPix_tot=0;
	double minfunc = 0;
    //int bweight = second ? 0 : in->pars().getBweight();
    int bweight = in->pars().getBweight();

	typename std::vector<Pixel<T> >::iterator pix;
	for(pix=anulus->begin();pix<anulus->end();pix++) {
		numPix_ring++;
		long x = pix->getX();
		long y = pix->getY();
		T theta = pix->getF();
        if (!getSide(theta)) continue;

		//< Factor for normalization.
        T modSum=0, obsSum = 0, factor=0;
		for (uint z=in->DimZ(); z--;) {
			long modPix = x+y*bsize[0]+z*bsize[0]*bsize[1];
			long obsPix = in->nPix(x+blo[0],y+blo[1],z);
			modSum += array[modPix];
			if (in->Array(obsPix)>0) obsSum += in->Array(obsPix)*mask[obsPix];
		}
		if (modSum!=0) factor = obsSum/modSum;
		else factor=0;
		
		double costh = fabs(cos(theta*M_PI/180.));
		double wi = std::pow(costh, double(wpow));

		// Normalizing and residuals.
		for (uint z=in->DimZ(); z--;) {
			long modPix = x+y*bsize[0]+z*bsize[0]*bsize[1];
			long obsPix = in->nPix(x+blo[0],y+blo[1],z);
			array[modPix] *= factor;	
			T obs = in->Array(obsPix)>0 ? in->Array(obsPix) : in->stat().getSpread();  
			T mod = array[modPix];
			
			if (mask[obsPix]==0 && mod==0) continue;
			else if (mask[obsPix]==0 && mod!=0) {
				numBlanks++;
				obs = in->stat().getSpread();
			}

			numPix_tot++;
            minfunc += getFuncValue(obs,mod,wi,chan_noise[z]);

		}	
	}
	
	//numPix_ring=1;
    //return std::pow((1+numBlanks/T(numPix_tot)),bweight)*minfunc/numPix_ring;
    return std::pow((1+numBlanks/T(numPix_tot)),bweight)*minfunc/((numPix_tot-numBlanks));

}
template double Galfit<float>::norm_local(Rings<float>*,float*,int*,int*);
template double Galfit<double>::norm_local(Rings<double>*,double*,int*,int*);
*/

template <class T>
double Galfit<T>::norm_azim (Rings<T> *dring, T *array, int *bhi, int *blo) {

    int bsize[2] = {bhi[0]-blo[0], bhi[1]-blo[1]};

    if (!in->StatsDef()) in->setCubeStats();

    int numPix_ring=0, numBlanks=0, numPix_tot=0;
    double minfunc = 0;
    int bweight = in->pars().getBweight();

    //< Factor for normalization.
    T obsSum=0, modSum=0, factor=1;
    for (uint y=bsize[1]; y--;) {
        for (uint x=bsize[0]; x--;) {
            double theta;
            if (!IsIn(x,y,blo,dring,theta)) continue;
            if (!getSide(theta)) continue;

            for (uint z=in->DimZ(); z--;) {
                long modPix = x+y*bsize[0]+z*bsize[0]*bsize[1];
                long obsPix = in->nPix(x+blo[0],y+blo[1],z);
                modSum += array[modPix];
                obsSum += in->Array(obsPix)*mask[obsPix];
            }
        }
    }

    if (modSum!=0) factor = obsSum/modSum;
    else factor=0;

    for (uint y=bsize[1]; y--;) {
        for (uint x=bsize[0]; x--;) {
            double theta;
            if (!IsIn(x,y,blo,dring,theta)) continue;
            if (!getSide(theta)) continue;
            numPix_ring++;
            double costh = fabs(cos(theta*M_PI/180.));
            double wi = std::pow(costh, double(wpow));

            // Normalizing and residuals.
            for (uint z=in->DimZ(); z--;) {
                long modPix = x+y*bsize[0]+z*bsize[0]*bsize[1];
                long obsPix = in->nPix(x+blo[0],y+blo[1],z);
                array[modPix] *= factor;
                T obs = in->Array(obsPix)>0 ? in->Array(obsPix) : in->stat().getSpread();
                T mod = array[modPix];

                if (mask[obsPix]==0) {
                    if (mod==0) continue;
                    else numBlanks++;
                }

                numPix_tot++;
                minfunc += getFuncValue(obs,mod,wi,chan_noise[z]);
            }
        }
    }
    //numPix_ring=1;
    //return std::pow((1+numBlanks/T(numPix_tot)),bweight)*minfunc/numPix_ring;
    return std::pow((1+numBlanks/T(numPix_tot)),bweight)*minfunc/((numPix_tot-numBlanks));

}
template double Galfit<float>::norm_azim(Rings<float>*,float*,int*,int*);
template double Galfit<double>::norm_azim(Rings<double>*,double*,int*,int*);


template <class T>
double Galfit<T>::norm_none (Rings<T> *dring, T *array, int *bhi, int *blo) {

    int bsize[2] = {bhi[0]-blo[0], bhi[1]-blo[1]};

    if (!in->StatsDef()) in->setCubeStats();

    int numPix_ring=0, numBlanks=0, numPix_tot=0;
    double minfunc = 0;
    int bweight = in->pars().getBweight();

    for (uint y=bsize[1]; y--;) {
        for (uint x=bsize[0]; x--;) {
            double theta;
            if (!IsIn(x,y,blo,dring,theta)) continue;
            if (!getSide(theta)) continue;

            numPix_ring++;

            double costh = fabs(cos(theta*M_PI/180.));
            double wi = std::pow(costh, double(wpow));

            // Normalizing and residuals.
            for (uint z=in->DimZ(); z--;) {
                long modPix = x+y*bsize[0]+z*bsize[0]*bsize[1];
                long obsPix = in->nPix(x+blo[0],y+blo[1],z);
                T obs = in->Array(obsPix)>0 ? in->Array(obsPix) : in->stat().getSpread();
                T mod = array[modPix];

                if (mask[obsPix]==0) {
                    if (mod==0) continue;
                    else numBlanks++;
                }

                numPix_tot++;
                minfunc += getFuncValue(obs,mod,wi,chan_noise[z]);
            }
        }
    }
    //numPix_ring=1;
    //return std::pow((1+numBlanks/T(numPix_tot)),bweight)*minfunc/numPix_ring;
    return std::pow((1+numBlanks/T(numPix_tot)),bweight)*minfunc/((numPix_tot-numBlanks));

}
template double Galfit<float>::norm_none(Rings<float>*,float*,int*,int*);
template double Galfit<double>::norm_none(Rings<double>*,double*,int*,int*);


template <class T> 
inline bool Galfit<T>::IsIn (int x, int y, int *blo, Rings<T> *dr, double &th) {
	
	// This function verifies that we are inside the rings.
	// Return also the value of azimutal angle th of (x,y) coordinates.
	
	double F = M_PI/180.;
	double pixScale = ((fabs(in->Head().Cdelt(0))*arcconv)+
					   (fabs(in->Head().Cdelt(1))*arcconv))/2.;
	T inc = dr->inc.back();
	T phi = dr->phi.back();
	double r1 = dr->radii.front()/pixScale;
	double r2 = dr->radii.back()/pixScale;
	double x0 = dr->xpos.back()-blo[0];
	double y0 = dr->ypos.back()-blo[1];
	double xr =  -(x-x0)*sin(F*phi)+(y-y0)*cos(F*phi);			
	double yr = (-(x-x0)*cos(F*phi)-(y-y0)*sin(F*phi))/cos(F*inc);
	double r = sqrt(xr*xr+yr*yr);
	if (r<0.1) th = 0.0;
	else th = atan2(yr, xr)/F;	
	return r>=r1 && r<=r2;			
	
}
template bool Galfit<float>::IsIn(int,int,int*,Rings<float>*,double&);
template bool Galfit<double>::IsIn(int,int,int*,Rings<double>*,double&);


template <class T>
inline bool Galfit<T>::getSide (double theta) {

    if (in->pars().getSIDE()=="R") return (fabs(theta)<=90.0);
    else if (in->pars().getSIDE()=="A") return (fabs(theta)>=90.0);
    else return true;

}
template bool Galfit<float>::getSide(double);
template bool Galfit<double>::getSide(double);


template <class T>
inline double Galfit<T>::getFuncValue(T obs, T mod, double weight, double noise_weight) {

    double value = 0;
    switch(in->pars().getFTYPE()) {
        case 1:
            value = weight*std::pow(mod-obs,2)/std::sqrt(obs)/noise_weight;
            break;
        case 2:
            value = weight*fabs(mod-obs)/noise_weight;
            break;
        case 3:
            value = weight*fabs(mod-obs)/(mod+obs)/noise_weight;
            break;
        case 4:
            value = weight*std::pow(mod-obs,2)/noise_weight;
            break;
        default:
            break;
    }
    return value;

}
template double Galfit<float>::getFuncValue(float,float,double,double);
template double Galfit<double>::getFuncValue(double,double,double,double);


template <class T>
std::vector<Pixel<T> >* Galfit<T>::getRingRegion (Rings<T> *dring, int *bhi, int *blo) {
	

	std::vector<Pixel<T> > *anulus= new std::vector<Pixel<T> >;
	long bsize[2] = {bhi[0]-blo[0], bhi[1]-blo[1]};
	bool *an = new bool[bsize[0]*bsize[1]];
	for (int i=0;i<bsize[0]*bsize[1];i++) an[i]=false;	
	
	T R1  = dring->radii.front()/(in->Head().PixScale()*arcconv);
	T R2  = dring->radii.back()/(in->Head().PixScale()*arcconv);
	T phi = dring->phi.back();
	T inc = dring->inc.back();
	T psi = 0.;
	T z0  = dring->z0.back()/(in->Head().PixScale()*arcconv); ///prima prendevo 3*dring->....
	T x0  = dring->xpos.back()-blo[0]-1;
	T y0  = dring->ypos.back()-blo[1]-1;
	
	double **matrices = RotMatrices(inc,psi,-phi-90);
	int size[2] = {3,3};
	double *rotmatrix = MatrixProduct(&matrices[2][0], size, &matrices[0][0],size);
	
	int xyrange = lround(R2);
	int zrange = lround(z0);
	int sizecoord[2] = {3,1};	
	for (int z=-zrange; z<=zrange; z++) {
		 for (int y=-xyrange; y<=xyrange; y++) {
			for(int x=-xyrange; x<=xyrange; x++) {
				double r = sqrt(x*x+y*y);
				if (r<=R2 && r>=R1) {
					double coord[3]={double(x),double(y),double(z)};
					double *coordrot = MatrixProduct(rotmatrix,size,coord,sizecoord);
					int xrot = lround(coordrot[0]+x0);
					int yrot = lround(coordrot[1]+y0);
					if (xrot>=0 && xrot<bsize[0] &&
						yrot>=0 && yrot<bsize[1]) {
						double theta;						
						if (r<0.1) theta = 0.0;
						else theta = atan2(y, x)/M_PI*180.;	
						if(!an[xrot+yrot*bsize[0]]) {
							an[xrot+yrot*bsize[0]] = true;
							Pixel<T> pix(xrot,yrot,theta);
							anulus->push_back(pix);
						}
					}
				}
			}
		}
	}

	delete [] an;
	deallocate_2D<double>(matrices,3);
	delete [] rotmatrix;
	return anulus;
	
}
template std::vector<Pixel<float> >* Galfit<float>::getRingRegion (Rings<float>*,int*,int*);
template std::vector<Pixel<double> >* Galfit<double>::getRingRegion (Rings<double>*,int*,int*);


template <class T>
T* Galfit<T>::getFinalRingsRegion () {
	

	long bsize[2] = {in->DimX(),in->DimY()};
	T *ringregion = new T[bsize[0]*bsize[1]];
	for (int i=0;i<bsize[0]*bsize[1];i++) ringregion[i]=log(-1);	
	
    T R1  = max(outr->radii.front()/(in->Head().PixScale()*arcconv)-outr->radsep/2.,0.);
    T R2  = outr->radii.back()/(in->Head().PixScale()*arcconv)+outr->radsep/2.;//#+sqrt(in->Head().BeamArea()/M_PI);
	T phi = outr->phi.back();
	T inc = outr->inc.back();
	T psi = 0.;
    T z0  = 3*outr->z0.back()/(in->Head().PixScale()*arcconv); //prima prendevo 3*dring->....
	T x0  = outr->xpos.back()-1;
	T y0  = outr->ypos.back()-1;
	
	double **matrices = RotMatrices(inc,psi,-phi-90);
	int size[2] = {3,3};
	double *rotmatrix = MatrixProduct(&matrices[2][0], size, &matrices[0][0],size);
	
	int xyrange = lround(R2);
	int zrange = lround(z0);
	int sizecoord[2] = {3,1};	
	for (int z=-zrange; z<=zrange; z++) {
		 for (int y=-xyrange; y<=xyrange; y++) {
			for(int x=-xyrange; x<=xyrange; x++) {
				double r = sqrt(x*x+y*y);
				if (r<=R2 && r>=R1) {
					double coord[3]={double(x),double(y),double(z)};
					double *coordrot = MatrixProduct(rotmatrix,size,coord,sizecoord);
					int xrot = lround(coordrot[0]+x0);
					int yrot = lround(coordrot[1]+y0);
					if (xrot>=0 && xrot<bsize[0] &&
						yrot>=0 && yrot<bsize[1]) {
						double theta;						
						if (r<0.1) theta = 0.0;
						else theta = atan2(y, x)/M_PI*180.;	
						if(isNaN(ringregion[xrot+yrot*bsize[0]])) {
							ringregion[xrot+yrot*bsize[0]] = theta;
						}
					}
				}
			}
		}
	}

	deallocate_2D<double>(matrices,3);
	delete [] rotmatrix;
	return ringregion;
	
}
template float* Galfit<float>::getFinalRingsRegion ();
template double* Galfit<double>::getFinalRingsRegion ();
}


#undef VROT
#undef VDISP
#undef DENS
#undef Z0
#undef INC
#undef PA
#undef XPOS
#undef YPOS
#undef VSYS
#undef VRAD
#undef MAXPAR
