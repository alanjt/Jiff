
#include "App.h"
#include <fstream>

App::App()
{
    NTable = 0;
    inputfilename="";

    gnuplot_out = "none";
    gnuplot_exe = GetGnuplotExeDefault();

    verbosityLevel = eLowVerb;

    version = "1.0";

}

bool App::File_Exists (std::string s_Path)
{
    std::ifstream in(s_Path.c_str(),std::ios::in);
    if (!in){
        in.close();
        return false;
    } else {
        in.close();
        return true;
    }
}
