/*                 
        cout << "JIFF: JSBSim Input File Formatter.\n\n";
        cout << "Usage: [-f] jiff.exe <input file> | [options]\n";
        cout << "Processes a JSBSim aircraft configuration file (JSBSim-ML Format v2.0 ) \n"
             << "producing Gnuplot script files (.plt).\n"
             << "Optionally a system call to gnuplot over the above scripts produces image files"
             << "in the user specified format.\n";
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <time.h>

#include "App.h"


int main(int argc, char *argv[])
{	

    App theApp;

    CmdParseStatus status = theApp.ProcessCmdArgs(argc, argv);

    if ( status == eGood ) 
    {
        cout << "JIFF: JSBSim Input File Formatter. Version " << theApp.GetVersion() << "\n";

        cout << "   Gnuplot output : " << theApp.GetGnuplotOut() << endl;
        cout << "   Gnuplot program: " << theApp.GetGnuplotExe() << endl;
        cout << "   cmd line parsed, now scanning XML file for tables ..."<<endl<<endl;

        cout << "Scanning for fdm_config" << endl;
        if ( theApp.ScanFile("fdm_config") )
        {
            cout<< "\nTotal Number of Tables found : NT = "<< theApp.GetNTable()<< endl;
            theApp.plot();
            return 0;
        }
        else 
	{
           cout << "Scanning for aerodynamics" << endl;
           if ( theApp.ScanFile("aerodynamics") )
           {
               cout<< "\nTotal Number of Tables found : NT = "<< theApp.GetNTable()<< endl;
               theApp.plot();
               return 0;
           }

	}
    } 
    else if  ( status == eBad ) 
    {
        cout << "JIFF: JSBSim Input File Formatter. Version " << theApp.GetVersion() << "\n";
        cout<< "Early termination. " << endl;
        cout<< "Try the command:\n\t jiff -h\nfor help." << endl;
        return 1;
    }
}
