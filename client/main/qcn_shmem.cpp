#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcn_shmem.h"
#include "str_util.h"  // boinc strlcpy etc
#include "qcn_util.h"
#include "util.h" // boinc, for dtime()

extern size_t strlcpy(char*, const char*, size_t);

CQCNShMem::CQCNShMem()
{
    clear(true);
}

CQCNShMem::~CQCNShMem()
{
	// if (strProjectPreferences) free(strProjectPreferences);
}

bool CQCNShMem::setTriggerLock()
{ // set the bTriggerLock mutex
   //static bool bInHere = false;
   //if (bInHere) return false; // another proc is in here
   //bInHere = true;
   int iLock = 0;
   while (bTriggerLock && iLock++ < 2000) {  // if bTriggerLock is false already, won't even go into this loop             
       usleep(1000L); // wait up to two seconds in .001 second increments
   }  
   if (!bTriggerLock) { // must have just become unlocked, so lock it again!
      bTriggerLock = true;
   } // note that bTriggerLock could have been set to true again in the nanosecond from the break statement?
   //bInHere = false;
   return bTriggerLock;
}

bool CQCNShMem::releaseTriggerLock()
{ // release the mutex bool
   bTriggerLock = false;
   return true; // this is always true
}

float CQCNShMem::averageDT()
{
	if (ullSampleCount)
	{
		return fRealDT / (float) ullSampleCount;
	}
	else
	{
		return 0.0f;
	}
}

float CQCNShMem::averageSamples()
{
	if (ullSampleCount)
	{
		return (float) ullSampleTotal / (float) ullSampleCount;
	}
	else
	{
		return 0.0f;
	}
}

// this resets the t0check & t0active, call right before you start accessing the sensor for mean_xyz
void CQCNShMem::resetSampleClock()
{
    t0active = dtime(); // use the function in boinc/lib
    if (t0startSession < 1.) t0startSession = t0active;
    t0start = t0active; // save the start time of the session
    t0check = t0active + dt;
#ifdef _DEBUG_QCNLIVE_WRAP
	// fake start time i.e. session started an hour and a half ago
	t0startSession = t0start - ((float)MAXI * g_DT);;
#endif	
    //sm->resetMinMax();
}

void CQCNShMem::clear(bool bAll)
{
    // don't zap dataBOINC, so copy over first
    if (bAll) { // clear it all, no need to save, this would be the first thing called, basically constructor
        memset(this, 0x00, sizeof(CQCNShMem));

		// set data points to the sac null value
		for (int i = 0; i < MAXI; i++) {
			x0[i] = y0[i] = z0[i] = fsig[i] = xa[i] = ya[i] = za[i] = SAC_NULL_FLOAT;
		}
		
		bMyVerticalTime = true;
		bMyVerticalTrigger = true;

		dMyLatitude = NO_LAT; 
		dMyLongitude = NO_LNG; 
		dMyElevationMeter = 0; 
		iMyElevationFloor = 0; 
		iMyAlignID = 0;

		dTrLatitude = NO_LAT; 
		dTrLongitude = NO_LNG; 
		dTrElevationMeter = 0; 
		iTrElevationFloor = 0; 
		iTrAlignID = 0;

		
#ifdef _DEBUG_QCNLIVE_WRAP
		// fill up the array with fake data to test
		double dTimeStart = dtime() - ((float)MAXI * g_DT);
		for (int i = 0; i < MAXI; i++) {
			x0[i] = ((float) rand() / (float) RAND_MAX) * 1.8f;
			y0[i] = ((float) rand() / (float) RAND_MAX) * .8f;
			z0[i] = ((float) rand() / (float) RAND_MAX) * 2.8f;
			xa[i] = ((float) rand() / (float) RAND_MAX) * 1.8f;
			ya[i] = ((float) rand() / (float) RAND_MAX) * .8f;
			za[i] = ((float) rand() / (float) RAND_MAX) * 2.8f;
			t0[i] = dTimeStart;
			dTimeStart += g_DT;
		}		
#endif

        // start with some values that really shouldn't be 0 ever
		//iTriggerLastElement = -1;
        dt = g_DT;
        fCorrectionFactor = 1.0f; // a fudge-factor in case the sensor is messed up i.e. JW24F14 calibration is not set to 2g range
        iWindow = (int) (g_cfTimeWindow / g_DT);  // number of points in time window
#ifndef QCNLIVE
		bMyOutputSAC = true;   // default to sac output for non qcnlive  (qcnlive uses the user prefs settings, defaults to non-sac i.e. csv/text output)
#endif
        return;
    }

    // for a 'not totally clear' - probably easiest to just make a "new copy"
    CQCNShMem* pshmem = new CQCNShMem();

    // copy over important, persistent fields we want to preserve
    bTriggerLock = true;
    pshmem->bTriggerLock = true;  // rudimentary locking

    pshmem->dt = dt; // this is the delta-time between point readings, currently .02 for the Mac sensor
    pshmem->lWrap = lWrap; // keeps count of how many times (without reset) we've been through the array (i.e. 1.5 hours
    pshmem->t0startSession  = t0startSession;  // save original start session time
    //strcpy(pshmem->strUploadLogical, strUploadLogical);  // path to the upload file
    //strcpy(pshmem->strUploadResolve, strUploadResolve);  // path to the upload file
    pshmem->iWindow = iWindow;
    pshmem->fCorrectionFactor = 1.00f; // reset correction factor
    strcpy(pshmem->strPathImage, strPathImage);  // path to the images

    pshmem->iNumTrigger = iNumTrigger;        // the total number of triggers for this workunit
    pshmem->iNumUpload  = iNumUpload;         // the total number of uploads for this workunit
    pshmem->iNumReset   = iNumReset;          // the number of timing resets this session has had (diags which can be trickled up)

	// qcnlive user pref vars
    pshmem->dTrLatitude = dTrLatitude; 
    pshmem->dTrLongitude = dTrLongitude; 
    pshmem->dTrElevationMeter = dTrElevationMeter; 
    pshmem->iTrElevationFloor = iTrElevationFloor; 
	pshmem->iTrAlignID = iTrAlignID;
    pshmem->dMyLatitude = dMyLatitude; 
    pshmem->dMyLongitude = dMyLongitude; 
    pshmem->dMyElevationMeter = dMyElevationMeter; 
    pshmem->iMyElevationFloor = iMyElevationFloor; 
	pshmem->iMyAlignID = iMyAlignID;
    pshmem->iMySensor = iMySensor; 
	pshmem->bMyContinual = bMyContinual;
	pshmem->bMyOutputSAC = bMyOutputSAC;
    strcpy(pshmem->strMyStation, strMyStation);
	pshmem->iMyAxisSingle = iMyAxisSingle;
	pshmem->bMyVerticalTime = bMyVerticalTime;
	pshmem->bMyVerticalTrigger = bMyVerticalTrigger;

    pshmem->clock_time  = clock_time;
	pshmem->cpu_time    = cpu_time;
	pshmem->update_time = update_time;
	pshmem->fraction_done = fraction_done;
	

	
    memcpy(&pshmem->statusBOINC, &statusBOINC, sizeof(BOINC_STATUS));

	// handle project prefs special - note dataBOINC.proj_pref would already have been freed
	if (dataBOINC.project_preferences) {
		strlcpy(strProjectPreferences, dataBOINC.project_preferences, SIZEOF_PROJECT_PREFERENCES);
		free(dataBOINC.project_preferences);
		dataBOINC.project_preferences = NULL;
	}

	memset(pshmem->strProjectPreferences, 0x00, SIZEOF_PROJECT_PREFERENCES);
	strlcpy(pshmem->strProjectPreferences, strProjectPreferences, SIZEOF_PROJECT_PREFERENCES);

	// now copy over, projectprefs is safely in strProjectPreferences
	// and we freed the memory from the dataBOINC.project_prefs
    memcpy(&pshmem->dataBOINC, &dataBOINC, sizeof(APP_INIT_DATA));

	// this "atomic" copy back should be safe
    memcpy(this, pshmem, sizeof(CQCNShMem));
    //resetMinMax();
    delete pshmem;
    bTriggerLock = false;
}

/*
void CQCNShMem::resetMinMax()
{
    //fmin[E_DX] = fmin[E_DY] = fmin[E_DZ] = fmin[E_DS] = std::numeric_limits<float>::max();
    //fmax[E_DX] = fmax[E_DY] = fmax[E_DZ] = fmax[E_DS] = std::numeric_limits<float>::min();
    fmin[E_DX] = fmin[E_DY] = fmin[E_DZ] = 10000.0;
    fmax[E_DX] = fmax[E_DY] = fmax[E_DZ] = -10000.0;
    fmin[E_DS] = fmax[E_DS] = 0.0; // sigma should be 0 min, set 0 max too;
}

void CQCNShMem::testMinMax(const float fVal, const e_maxmin eType)
{
   if (fVal < fmin[eType]) fmin[eType] = fVal;
   else if (fVal > fmax[eType]) fmax[eType] = fVal;
}
*/

double CQCNShMem::TimeError()
{
  return  ((t0active-t0check) / dt) * 100.0f;  // timing error relative to the (usually) 0.02 second dt window
}


