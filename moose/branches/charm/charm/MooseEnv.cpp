#include <errno.h>
#include <iostream>

using namespace std;

#include "MooseEnv.h"

string MooseEnv::PreviousDirectory_ = "..";
string MooseEnv::DirectorySeparator_ = "/";
string MooseEnv::CurrentWorkingDirectory_ = MooseEnv::getcwd_();
string MooseEnv::FlagPrefix_ = "--";

// adapted from 
// http://www.gnu.org/software/libc/manual/html_node/Working-Directory.html
string MooseEnv::getcwd_(){
  size_t size = 100;

  while(1){
    char *buffer = new char[size];
    if (getcwd (buffer, size) == buffer){
      string ret = string(buffer);
      delete[] buffer;
      return ret;
    }

    if (errno != ERANGE)
      return string("");

    size *= 2;
  }
}

bool MooseEnv::isAbsolutePath(const string &path){
  return path.substr(0, 1) == DirectorySeparator_;
}

unsigned int MooseEnv::getNumCores()
{
#ifdef USE_CHARMPP
  // XXX don't use this function in charm++ version
  return 1;
#endif
	unsigned int numCPU = 0;
#ifdef WIN_32
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	numCPU = sysinfo.dwNumberOfProcessors;
#endif

#ifdef LINUX
	numCPU = sysconf( _SC_NPROCESSORS_ONLN );
#endif

#ifdef MACOSX
	int mib[4];
	size_t len = sizeof(numCPU); 

	/* set the mib for hw.ncpu */
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

	/* get the number of CPUs from the system */
	sysctl(mib, 2, &numCPU, &len, NULL, 0);

	if( numCPU < 1 ) 
	{
		mib[1] = HW_NCPU;
		sysctl( mib, 2, &numCPU, &len, NULL, 0 );
	}
#endif
	if ( numCPU < 1 )
	{
		cout << "No CPU information available. Assuming single core." << endl;
		numCPU = 1;
	}
	return numCPU;
}


