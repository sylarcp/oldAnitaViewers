/*
 * trigmondefs.h
 */
#include <includes/anitaStructures.h>
#ifndef _TRIGMONDEFSH_
#define _TRIGMONDEFSH_

#ifdef DEBUG
#include <iostream>

using namespace std;
#endif

#define TITLE _T("TrigMon-III - Anita-III Master Trigger Monitor")
#define DEFAULT_OPTIONS_FILE "default.trc"
#define TM_FTSIZE 9

//#define N_SURF 9
#define N_SURF ACTIVE_SURFS  //amir's addition july 15 08
//#define N_BAND 32
//#define N_BAND 12 // changed on 7-Oct-14, SM.
#define N_CHN 9
#define ANTS_PER_SURF 4
#define N_PHI_SECTORS 16
#define ANT_LAYERS 3      // added 9-Oct-14,  SM.

#define N_L1 N_PHI_SECTORS
#define L1_PER_ROW N_PHI_SECTORS
#define L1Id(i,j) ((i)+(j)*L1_PER_ROW)
//#define N_L2 (2*N_PHI_SECTORS)
#define N_L2 (2*N_PHI_SECTORS+16)//amir's addition july 15 08
//#define L2_PER_ROW N_PHI_SECTORS
#define L2_PER_ROW (N_PHI_SECTORS)//amir's addition july 15 08
#define L2Id(i,j) ((i)+(j)*L2_PER_ROW)
#define N_L3 N_PHI_SECTORS
#define L3_PER_ROW N_PHI_SECTORS
#define L3Id(i,j) ((i)+(j)*L3_PER_ROW)

#define N_RFPOWER (N_SURF*(N_CHN-1))
#define RFPOWER_PER_ROW (N_CHN-1)
#define RFPOWERId(i,j) ((i)+(j)*RFPOWER_PER_ROW)
//#define N_SCALER ((N_SURF-1)*N_BAND)
#define SCALER_PER_ROW 12
#define N_SCALER (N_SURF*SCALER_PER_ROW)
#define SCALERId(i,j) ((i)+(j)*SCALER_PER_ROW)
//#define N_THRESHOLD ((N_SURF-1)*N_BAND)
#define THRESHOLD_PER_ROW SCALER_PER_ROW
#define N_THRESHOLD (N_SURF*THRESHOLD_PER_ROW)
#define THRESHOLDId(i,j) ((i)+(j)*THRESHOLD_PER_ROW)

#define MAX_SPECS 10

#define SCALER_GOAL_PRECISION 0.05

#endif // _TRIGMONDEFSH_
