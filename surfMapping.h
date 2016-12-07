/*
* surfMapping.h
*/

// This file is map between antenna number in internal numbering scheme and
// corresponding RF channel, and also phi sector

#include "aviewdefs.h"


/* Maping per GV email of 05/22/06, in 1-counting mode for SURFs,
  1-counting for channels; 1-counting for phi sectors
  Each entry {H_SURF,H_CHAN,V_SURF,V_CHAN,PHI};
  i.e. H_SURF==Horizontal channel SURF, etc.
  For Veto's and Nadirs, {SURF,CHAN,SURF,CHAN,0}; second being redundant
*/
struct SurfMapType surfMap[]={
{ 12 , 8 , 12 , 4 , 1 },
{ 11 , 8 , 11 , 4 , 3 },
{ 12 , 6 , 12 , 2 , 5 },
{ 11 , 6 , 11 , 2 , 7 },
{ 12 , 7 , 12 , 3 , 9 },
{ 11 , 7 , 11 , 3 , 11 },
{ 12 , 5 , 12 , 1 , 13 },
{ 11 , 5 , 11 , 1 , 15 },
{ 6 , 6 , 6 , 2 , 2 },
{ 5 , 2 , 5 , 6 , 4 },
{ 5 , 8 , 5 , 4 , 6 },
{ 6 , 8 , 6 , 4 , 8 },
{ 6 , 5 , 6 , 1 , 10 },
{ 5 , 5 , 5 , 1 , 12 },
{ 5 , 7 , 5 , 3 , 14 },
{ 6 , 7 , 6 , 3 , 16 },
{ 10 , 6 , 10 , 2 , 1 },
{ 4 , 8 , 4 , 4 , 2 },
{ 9 , 6 , 9 , 2 , 3 },
{ 3 , 8 , 3 , 4 , 4 },
{ 9 , 8 , 9 , 4 , 5 },
{ 4 , 6 , 4 , 2 , 6 },
{ 10 , 8 , 10 , 4 , 7 },
{ 3 , 6 , 3 , 2 , 8 },
{ 10 , 5 , 10 , 1 , 9 },
{ 4 , 7 , 4 , 3 , 10 },
{ 9 , 5 , 9 , 1 , 11 },
{ 3 , 7 , 3 , 3 , 12 },
{ 9 , 7 , 9 , 3 , 13 },
{ 4 , 5 , 4 , 1 , 14 },
{ 10 , 7 , 10 , 3 , 15 },
{ 3 , 5 , 3 , 1 , 16 },
{ 7 , 8 , 7 , 4 , 1 },
{ 1 , 6 , 1 , 2 , 2 },
{ 8 , 8 , 8 , 4 , 3 },
{ 2 , 6 , 2 , 2 , 4 },
{ 7 , 6 , 7 , 2 , 5 },
{ 2 , 8 , 2 , 4 , 6 },
{ 8 , 6 , 8 , 2 , 7 },
{ 1 , 8 , 1 , 4 , 8 },
{ 7 , 7 , 7 , 3 , 9 },
{ 1 , 5 , 1 , 1 , 10 },
{ 8 , 7 , 8 , 3 , 11 },
{ 2 , 5 , 2 , 1 , 12 },
{ 7 , 5 , 7 , 1 , 13 },
{ 2 , 7 , 2 , 3 , 14 },
{ 8 , 5 , 8 , 1 , 15 },
{ 1 , 7 , 1 , 3 , 16 }

};


