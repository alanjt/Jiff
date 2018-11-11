#include "App.h"
#include "FGJSBBase.h"
#include "input_output/FGXMLParse.h"
#include "input_output/FGXMLElement.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <iomanip>

string App::makeplt(myPlot P)
{

    std::ofstream pltfile(P.pltfile.c_str());
    pltfile.seekp( 0, std::ios::beg);
    
    pltfile << "#! " << GetGnuplotExe() << endl;
    pltfile << "set title \""<< P.title <<"\"" << endl
            << "set xlabel \""<< P.xlabel <<"\"" << endl
            << "set ylabel \""<< P.ylabel <<"\"" << endl
            << "set term " << P.terminal << endl
            << P.keycmd << endl
            << P.indexopt << endl;
    if (P.output != "" ) 
        pltfile << "set out '" << P.output << "'" << endl;
    pltfile << "plot \\" << endl;

    for ( unsigned int r=0; r < P.plotrows.size(); r++) {
        pltfile << P.plotrows[r];
        if (r<(P.plotrows.size()-1))
            pltfile << ", \\" << endl;
        else
            pltfile << endl;
    }
    pltfile << "set out" << endl
            << "set term " << P.terminal << endl;
// wag      << "set term win" << endl;

    
    pltfile.close(); 
    return P.pltfile;
}

void App::plot(void)
{
    myTable T;
    myPlot P;

    string pltfilename, command;
	cout << "\nplot \n " ;

    vector <myTable>::iterator tableIter;

    for ( tableIter=tableVec.begin(); tableIter!=tableVec.end(); tableIter++ )
    {
        P.Clear();

        myTable T= *tableIter;
        cout<< T.tablename << "plot   nRows= " << T.nRows << endl;

        // populate plot struct
        
        P.pltfile = T.tablename+".plt";
        P.datfile = T.tablename+".dat";

        string plotout = GetGnuplotOut();
        if ( plotout  == "none" ) {
#ifdef unix
            P.terminal = "x11";
#else
            P.terminal = "windows color";
#endif
            P.output   = "";
        } else if ( plotout == "ps" ) {
            P.terminal = "postscript color \"Helvetica 12\"";
            P.output   = T.tablename+".ps";
        } else if ( plotout == "png" ) {
            P.terminal = "png medium size 1024,768 crop";
            P.output   = T.tablename+".png";
        } else if ( plotout == "svg" ) {
            P.terminal = "svg";
            P.output   = T.tablename+".svg";
        } else {
	    P.terminal = plotout;
            P.output   = T.tablename + "." + plotout;
	}

        P.title = T.tablename;
        if ( T.nIVars == 2 )
            P.title += ", various values of " + T.iVars["row"]; // T.iVars["column"];

		if (T.nIVars == 3)
			P.title += ", various values of " + T.iVars["row"] + " , " + T.iVars["table"] + " = " + T.skeyname;

        if ( T.nIVars == 1 )
            P.xlabel = T.iVars.begin()->second;
        if ( T.nIVars >= 2 )  
            P.xlabel = T.iVars["column"]; // T.iVars["row"];

        P.ylabel = T.funcname;

        if ( T.nIVars == 1 ) {
            P.keycmd = "unset key";
            P.indexopt = "";
            P.plotrows.push_back("'"+P.datfile+"'"+" w linespoints lw 2");
        }
        if ( T.nIVars == 2 ) {
            P.keycmd = "set key top left";
            //unsigned nv = T.vParRow.size(); // T.ParValues.size();
            unsigned nv = T.vParColumn.size(); // T.ParValues.size();
            for ( unsigned int c=0; c < nv ; c++) {
                std::ostringstream oss;
                oss << "'" << T.tablename << ".dat'" << " index " << c
                    //<< " t '" << T.ParValues[c] << "' w linespoints lw 2";
                    //<< " t '" << T.vParRow[c] << "' w linespoints lw 2";
                    << " t '" << T.vParColumn[c] << "' w linespoints lw 2";
                P.plotrows.push_back(oss.str());
			}

		}
		if (T.nIVars == 3) {
			P.keycmd = "set key top left";
			//unsigned nv = T.vParRow.size(); // T.ParValues.size();
			unsigned nv = T.vParColumn.size(); // T.ParValues.size();
			for (unsigned int c = 0; c < nv; c++) {
				std::ostringstream oss;
				oss << "'" << T.tablename << ".dat'" << " index " << c
					//<< " t '" << T.ParValues[c] << "' w linespoints lw 2";
					//<< " t '" << T.vParRow[c] << "' w linespoints lw 2";
					<< " t '" << T.vParColumn[c] << "' w linespoints lw 2";
				P.plotrows.push_back(oss.str());
			}

		}

		pltfilename = makeplt(P);

		if ( GetGnuplotOut() != "none" )
		{
			command = GetGnuplotExe() + " " + pltfilename;
//			 cout << "Executing command: " << command << "\n" << endl;
			system(command.c_str());
		}
    };
    return;
}
