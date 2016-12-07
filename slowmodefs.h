/*
 * slowmodefs.h
 */
#include <includes/anitaStructures.h>
#ifndef _SLOWMODEFSH_
#define _SLOWMODEFSH_

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#define TITLE _T("SlowMo-III - Anita-III Slow Telemetry Monitor")
#define DEFAULT_OPTIONS_FILE _T("default.src")

#define N_SURF ACTIVE_SURFS
#define N_CHN 9
#define ANTS_PER_SURF 4
#define N_PHI_SECTORS 16

#define N_TEMP 4
#define N_POWER 4

#define N_L1 (N_SURF)
#define L1_PER_ROW (N_SURF)
#define L1Id(i,j) ((i)+(j)*L1_PER_ROW)
#define N_L2 (N_PHI_SECTORS) //amir's addition august 08 08
#define L2_PER_ROW N_PHI_SECTORS
#define L2Id(i,j) ((i)+(j)*L2_PER_ROW)
#define N_L3 N_PHI_SECTORS
#define L3_PER_ROW N_PHI_SECTORS
#define L3Id(i,j) ((i)+(j)*L3_PER_ROW)

#define N_RFPOWER (N_SURF*(N_CHN-1))
#define RFPOWER_PER_ROW (N_CHN-1)
#define RFPOWERId(i,j) ((i)+(j)*RFPOWER_PER_ROW)
#define N_SCALER ((N_SURF)*(N_CHN-1))
//#define N_SCALER ((N_SURF)*ANTS_PER_SURF)
#define SCALER_PER_ROW (N_SURF)
#define SCALERId(i,j) ((i)+(j)*SCALER_PER_ROW)
#define N_SCALERRMS ((N_SURF)*(N_CHN-1))
//#define N_SCALERRMS ((N_SURF)*ANTS_PER_SURF)
#define SCALERRMS_PER_ROW (N_SURF)
#define SCALERRMSId(i,j) ((i)+(j)*SCALERRMS_PER_ROW)

#define MAX_SPECS 10

#endif // _SLOWMODEFSH_
