/*! \file anitaStructures.h
  \brief Contains the definitions of all the structures used to store 
  and move ANITA data and stuff.
    
  Some simple definitions of the structures that we'll use to store 
  ANITA data. In here there will be the structures used for transient data,
  housekeeping stuff, GPS stuff and anything else that is going to be used
  by more than one of the processes. This will all change to use bit fields,
  at some point in the future. For now this is just a place holder we can
  test data flow with.

  August 2004  rjn@mps.ohio-state.edu
*/
#ifndef _GSE_STRUCTURES_H
#define _GSE_STRUCTURES_H
#include "includes/anitaFlight.h"

typedef struct {
    	double accx[2],accy[2],accz[2],acct[2];
	 double ssel[4],ssaz[4],sst[4];
 	 long ssflag[4];
	 float pressh,pressl;
	 float p1_5v,p3_3v,p5v,p5sbv,p12v,p24v,ppvv,n5v,n12v,iprf1v,iprf2v;
	 float p1_5i,p3_3i,p5i,p5sbi,p12i,p24i,ppvi,n12i,iprf2i,bati;
	 double it[25],et[25];
	 long time;
	 float magx,magy,magz;
	 long cpu1,cpu2,core1,core2,disk0,disk1,disk2,disk3,disk4,disk5;
        int crc;
} DBHK_t;

typedef struct {
    	 long time,ns,us,runnum,antmask,evnum;
	 int crc,surfmask,calib,priority,turfword,trigtype,trignum,l3cnt;
	 long c3po,trigtime,pps,deadtime;
         int l3trigpat, l3trigpatH, phimask, phimaskh;
} DBHeader_t;
typedef struct {
    	 long time;
	 int crc;
	 long disk[8];
	// char blade[12];
    	// char usbint[12];
    	// char usbext[12];
	 long linkev[NUM_PRIORITIES];
} DBMon_t;
typedef struct {
    	 long time;
	 int crc,numCmdBytes,flag;
	 long cmdLong[MAX_CMD_LENGTH];
	
} DBCmd_t;

#endif /* ANITA_STRUCTURES_H */

