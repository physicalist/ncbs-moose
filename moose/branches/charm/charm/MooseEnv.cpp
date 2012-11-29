#include <errno.h>

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
