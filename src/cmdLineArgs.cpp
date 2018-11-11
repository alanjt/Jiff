/* 
 *          Process main command line arguments
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <fstream>

using namespace std;

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
using namespace boost;
namespace po = boost::program_options;

#ifndef PROPS_STANDALONE
#define PROPS_STANDALONE 0
#endif

#include <math/FGTable.h>

#include "App.h"

#ifdef unix
  const char* progname = "jiff";
#else
  const char* progname = "jiff.exe";
#endif

//bool 
CmdParseStatus 
App::ProcessCmdArgs( int argc, char **argv )
{
    //bool 
    CmdParseStatus status = eGood;

    int ac = argc;
    char **av = argv;

    po::positional_options_description p;
    p.add("input-file", -1);

    // Declare options allowed on command line
    po::options_description genericOpts("Generic options");
    genericOpts.add_options()
       ("help,h", "produce help message")
       ("version,v", "print version string")
       ("verbosity,V", po::value<string>(),
             "<string>. Verbosity level. {\"low\" (default) | \"high\"} ")
       ("input-file,f", po::value<string>(),
             "<string>. Input file containing XML fdm config file")
       ("gnuplot-output,g", po::value<string>(),
       "<string>. Type of gnuplot output. "
        "Supported values: \"none\" (default), \"ps\", \"png\", \"svg\" ...\n"
	"although other gnuplot output devices may work; try at your own risk.\n"
	"When this option is not \"none\", a system call to the Gnuplot executable is done;  "
	"see option -G below.")
       ("gnuplot-exe,G", po::value<string>(),
       ("<string>. Full path to gnuplot executable. Default is \""
       + GetGnuplotExeDefault() + "\"").c_str())
       ;

    po::options_description cmdline_options;
    cmdline_options.add(genericOpts);     

    // The map of all the options, i.e. the main "storage component"
    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(cmdline_options).positional(p).run(), vm);

    // -help option
    if (vm.count("help")) 
    {
#if defined(_MSC_VER)
        cout << "JIFF: JSBSim Input File Formatter.\n\n";
        cout << "Usage: [-f] " << progname << " <input file> | [options]\n";
        cout << "Processes a JSBSim aircraft configuration file (JSBSim-ML Format v2.0 ) \n"
             << "producing Gnuplot script files (.plt).\n"
             << "Optionally a system call to gnuplot over the above scripts produces image files"
             << "in the user specified format.\n";
        /*
            vc80 is crashes at this point:
            cout << configOpts.options() << endl;
        */
        /*
        typedef boost::shared_ptr<po::option_description> spOpts_t;
        typedef const std::vector< spOpts_t >& v_rOpts_t;
        v_rOpts_t v_rOpts = configOpts.options();
        cout << v_rOpts << endl;
        */

        // cfr. <boost/program_options/options_description.hpp> line 193
        const std::vector< boost::shared_ptr<po::option_description> >& opts 
            = genericOpts.options();

        cout << endl << "Options:" << endl << endl;
        for(unsigned int i=0; i<opts.size(); i++)
        {
            boost::shared_ptr<po::option_description> sp = opts[i];
            //cout << sp.get() << endl;     // pointer to an option_description
            //cout << *(sp.get()) << endl;  // an option_description
            //cout << "\t"<< (*(sp.get())).description() << endl;
            string str = (*(sp.get())).format_name()
                          +" "
                          +(*(sp.get())).format_parameter();
            cout << str << endl;
            cout << (*(sp.get())).description() << endl << endl;
        }

#else
      
    cout << "JIFF: JSBSim Input File Formatter.\n\n";
    cout << "Usage: [-f] " << progname << " <input file> | [options]\n";
    cout << "Processes a JSBSim aircraft configuration file (JSBSim-ML Format v2.0 ) \n"
         << "producing Gnuplot script files (.plt).\n"
         << "Optionally calls gnuplot using the above scripts to produce image files "
         << "in the user specified format.\n";

    cout << genericOpts << "\n";
#endif
        status = eHelp;
        return status;
     };

    // version option   
    if (vm.count("version")) 
    {
        cout << "Version" << version << "(March 2016).\n\n";
        status = eHelp;
        return status;
    };

    // verbpsity level
    if (vm.count("verbosity"))
    {
        string s = vm["verbosity"].as<string>();
        verbosityLevel = eLowVerb;
        if ( s == "low")
            verbosityLevel = eLowVerb;
        if ( s == "high")
            verbosityLevel = eHighVerb;
    }

    // type of gnuplot output
    if (vm.count("gnuplot-output"))
    {
        string s = vm["gnuplot-output"].as<string>();
        cout << "user specified gnuplot output: ";
        if (  ( s != "none")
            &&( s != "ps")
            &&( s != "jpeg")
            &&( s != "png")
            ) {
                gnuplot_out = "none";
                cout << s << " rejected, reset to \"none\"." << endl;
        } else
            cout << s << endl;
            gnuplot_out = s;
    }

    // gnuplot executable (full path included)
    if (vm.count("gnuplot-exe"))
    {
        string s = vm["gnuplot-exe"].as<string>();
        cout << "user specified gnuplot exec: " << s << endl;

        if( !File_Exists(s) )
        {
            cout << "Rejected: unable to find file "<< s << endl;
//wag
                cout << "Under Cygwin, you have to specify the Windows path" << endl;
                cout << "   such as c:/cygwin/bin/gnuplot.exe" << endl;
//wag end                
            cout << "Gnuplot output reset to \"none\"." << endl;
            gnuplot_out = "none";
            gnuplot_exe = GetGnuplotExeDefault();
        }
        else {
            gnuplot_exe = s;
            cout << "File found." << endl;
        }
    }

    // look for input file
    if (vm.count("input-file"))
    {
        string s=vm["input-file"].as<string>();
        if( !File_Exists(s) )
        {
            cout<<"    No input file found in current directory    "<<endl;
            status = eBad;
        }
        else
        {
            SetInputFileName(s);
            cout << "   Input file set to  \"" << s << "\"\n";
        };
    }
    else
    {
        cout << "no input file specified\n\n";
        status = eBad;
    }

    // end of options count
    return status;
}
