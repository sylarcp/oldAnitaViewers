********
**  ANITA Viewer
********

This is ANITA data viewer. It will display raw data (as output by
Shige's crate_eval), flight data (as output by flight software) and
it can retrieve data from ANITA PostgreSQL database.

The program is build on wxWidget toolkit, version 2.6.3. To compile,
in addition you will need libfftw3 library (see www.fftw.org), libpq
library version 7.4.11 or compatible (see www.postgresql.org), and
libexpat (see expat.sourceforge.net). You'll need associated headers
as well.  Also, you will need make anitaFlight.h and
anitaStructures.h files and CompressLib, UtilLib, and PedestalLib
libraries available.  They are part of Anita flight software
distribution, common/ subdirectory.  You can get and abbreviated
version at
http://www.phys.hawaii.edu/~predrag/post/anita/software/flightSoft_common.tgz
Untar it at one directory above this one and follow these steps;

(Use http://www.phys.hawaii.edu/~shige/post/anita/flightSoft.tgz,
instead. Because the original one in Ped's area contains an older
version of anitaStructures.h   15-Jan-08, SM)

tar xzf flightSoft_common.tgz
cd flightSoft
declare -x ANITA_FLIGHT_SOFT_DIR=$PWD
cd common
make
declare -x FLIGHTSOFT_LIBDIR=$ANITA_FLIGHT_SOFT_DIR/lib


To build on AnitaViewer on Linux, just type make. If you run into
problems make sure all required libraries and headers are visible
and adjust Makefile flags accordingly. You can build in debug and/or
static modes as well. Type 'make DEBUG=yes STATIC=yes'. Static
linking will produce some warnings, but you can ignore
them. Executable will be portable. In either, static or dynamic
linking case, the executable will be created in GTK2/ subdirectory.

To build on Windows, you will need cygwin or mingw environment
installed, with libraries listed above available. There are known
problems with compiling flightSoft libraries under Windows, so
contact Ryan Nichol or Ped Miocinovic if you can't resolve them
yourself. If you're installing PostgreSQL on windows, you only need
to compile and install client/ subdirectory which provides libpq
(the full compilation will likely fail). Then run 'make WIN=yes'.  I
suggest you compile all dependencies (i.e. wxWidgets, fftw,
PostgreSQL) in static mode on Windows. Also, wxWidgets will compile
its own expat library if it doesn't find one on the system
already. The executables will be created in MSW/ subdirectory.

Linux version comes with TrigMon GUI for in-depth display of trigger 
information. It will be built by default. 

Hopefully, using Anita Viewer will be intuitive, but I hope to write a 
brief user manual at one point. 

-- Ped 09/27/06

** Compiling and running on RHEL, FC, and Debian.    added 29-Jan-08 

While compiling AnitaViewer on a machine with RHEL 5 (or
equivalent), FC (6 or later), or Debian 4 (Etch), it is necessary to
take some cautions. Namely, you have to install various libraries,
incliuding a system specific flavor of wxWidget library.  (Also,
there is a compliactions due to the fact that the libraries supplied
are UNICODE enabled, though all the UNICODE related changes are made
on the code, I hope.) I've compiled AnitaViewer on the machines with
Scientific Linux 5.0, Fedora 7, and Debian 4 (Etch) successfully.
Listed below are libraries needed on each flavor of linux's.

1) Scientific Linux 5.0

 This is a clone of RHEL 5.0 (therefore, CentOS 5.0, etc.) and based
on FC6. To compile AnitaViewer on this platform, you need to

 - install "wxGTK-devel" using yum.

2) Fedora 7 (and possiblly later)

 - install "compat-wxGTK26-devel" using yum

3) Debian

 - install wx-common and libwxgtk2.6-dev using apt-get

For all distributions, you have to make sure that fftw3, postgresql
and their headers (*-devel for RH and *-dev for Debian) are
installed along with c/c++ compiler. Also, you have to let the
system know where Anita libraries are by using a "ldconf
$ANITA_FLIGHT_SOFT_DIR/lib" command as a root user, befoer running
the Anitaviewer.

-- SM

