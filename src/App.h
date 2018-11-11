#ifndef APP_H
#define APP_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <FGJSBBase.h>
#include <input_output/FGXMLParse.h>
#include <input_output/FGXMLElement.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;


enum CmdParseStatus { eHelp = 0, eGood, eBad };

enum Verbosity { eLowVerb = 0, eNormVerb, eHighVerb };

struct myTable
{
    string funcname, tablename, skeyname;
    //JSBSim::Element tablePtr;
    std::map<string,string> iVars;
    int nRows;
    int nColumns;
    int nIVars;
    std::vector<float> ParValues;
    vector<float> vParRow, vParColumn;
    vector< vector<float> > m;
};

struct myPlot
{
    string pltfile;
    string datfile;
    string output;
    string terminal;
    string title;
    string keycmd;
    string xlabel;
    string ylabel;
    string indexopt;
    vector<string> plotrows;

    void Clear(){
        pltfile  = "";
        datfile  = "";
        output   = "";
        terminal = "";
        title    = "";
        keycmd   = "";
        xlabel   = "";
        ylabel   = "";
        indexopt = "";
        plotrows.clear();
    };
};

class App
{
public:
    App();
    ~App(){};

    //bool 
    CmdParseStatus ProcessCmdArgs( int argc, char **argv );
    bool ScanFile(string);
    inline void SetInputFileName( std::string f ) { inputfilename=f; };
    inline int GetNTable(void) {return NTable;};
    void plot(void);
//	void PrintT(Element *el, unsigned int level=0);
	void PrintT(JSBSim::Element *el, unsigned int level=0);
	void TableT(JSBSim::Element *el);

    inline std::string GetGnuplotOut(void) const {return gnuplot_out;};
    inline std::string GetGnuplotExe(void) const {return gnuplot_exe;};
    inline std::string GetGnuplotExeDefault(void) const {
#ifdef unix
// TODO: FIXME: this should be set by autoconfigure:
      return "/usr/bin/gnuplot";
#else
//#error bar
//      return "pgnuplot.exe";
      return "gnuplot.exe";
#endif      
    }

    inline std::string GetVersion(void) const {return version;};

private:

    bool File_Exists (std::string s_Path);
    std::string inputfilename;
    int NTable;
    vector <myTable> tableVec;

    std::string gnuplot_out;
    std::string gnuplot_exe;

    string makeplt(myPlot P);

    Verbosity verbosityLevel;

    string version;
};
#endif
