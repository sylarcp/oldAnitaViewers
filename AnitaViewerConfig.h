#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iomanip>

#ifdef LINUX
#include <unistd.h>
#endif
using namespace std; // Needed to use C++ standard library.

#ifdef LINUX
#include <libpq-fe.h>
#endif

#ifdef WIN32
#include "libpq-fe.h"
#endif

#define TIMER_INTERVAL 10000
#define DB_CONNECT_ERROR 1
#define WAVEFORM_VALUES 255
#define WAVEFORM_DISPLAY_PIXEL_WIDTH 630
#define WAVEFORM_DISPLAY_PIXEL_HEIGHT 80

/*
 * Index and value structure.
 * Used for time domain data.
 *
 * i = index, v = value
 */
struct index_value {
    int i;
    double v;
};
