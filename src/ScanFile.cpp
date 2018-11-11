#include "App.h"
#include <FGJSBBase.h>
#include <input_output/FGXMLParse.h>
#include <input_output/FGXMLElement.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
#include <utility>

using namespace JSBSim;

#include <boost/algorithm/string.hpp>

	std::pair<string,string> mapPair;

	Element *document = 0L, *aeroelement = 0L, *funelement = 0L,
		*axelement = 0L, *tabelement = 0L,
		*productelement = 0L,
		*xmlelement = 0L, *xmlelement2 = 0L,
		*iVarPtr = 0L, *brkptPtr = 0L;
	string lookup;
	myTable T;
	int debug;

void App::PrintT(Element *el, unsigned int level)
{
	unsigned int i,j, spaces, numlines, numchildren;
	Element *childelement = 0L;

	level +=2;
	for (spaces=0; spaces<=level; spaces++)cout << " "; // format output
	cout << "\nPrintT level " << level << ", Element Name: " <<el->GetName() <<endl;
	for (spaces=0; spaces<=level; spaces++)cout << " "; // format output
	numlines = el->GetNumDataLines();
	cout << "PrintT No of Data Lines: "<<numlines<<endl;
	for (i=0; i < numlines; i++)
		{
		for (spaces=0; spaces<=level; spaces++)cout << " "; // format output
		cout << "PrintT DataLine: " <<el->GetDataLine(i)<<endl;
		}
	if (el->GetName() == "table")
		{
			for (spaces=0; spaces<=level; spaces++)cout << " "; // format output
			cout<<"PrintT table found: "<<el->GetAttributeValue("name")<<endl;

			TableT(el);  // write table to .dat file(s)
		}
	for (spaces=0; spaces<=level; spaces++)cout << " "; // format output
	numchildren = el->GetNumElements();
	cout << "PrintT No of Children: "<<numchildren<<endl;

	if (numchildren > 0)
		{
		
		for (j=0; j < numchildren; j++)
			{
			childelement = el->GetElement(j);
			for (spaces=0; spaces<=level; spaces++)cout << " "; // format output
			cout << "PrintT Child "<<j<<" : "<<childelement << endl;
			// recursive call to this function to find all children, grandchildren, greatgrandchildren etc etc.
			PrintT(childelement,level); 
			}
		}
}


void App::TableT(Element *el)
 {
	Element *childelement = 0L;
	Element *tabledataEl = 0L;

//	NTable+=1;
	string name = el->GetAttributeValue("name");
	string funcname = el->GetAttributeValue("name");
	string descript=el->GetAttributeValue("description");
	int numrows = el->GetNumDataLines();

	for (string::iterator nn = name.begin(); nn != name.end(); nn++)
	{
		if (!isalnum(*nn)) *nn = '_';
	}
	unsigned int where = name.find_last_not_of("_");
	if (where != string::npos) name.resize(1+where);
	cout << "TableT 	Description: " << descript << endl;

	T.nRows = numrows;
	T.funcname = funcname;
	T.tablename = name;
	T.nIVars=0;
	T.iVars.clear();
	iVarPtr=el->FindElement("independentVar");
	while (iVarPtr)
	{
		string s0 = iVarPtr->GetAttributeValue("lookup");
		if (s0.empty()) s0 = "row";
		lookup = s0;
		string s = iVarPtr->GetDataLine();
		if ( lookup == "row" ) T.iVars["row"] = s;
		if ( lookup == "column" ) T.iVars["column"] = s;
		if (lookup == "table") T.iVars["table"] = s;
		T.nIVars+=1;
		iVarPtr=el->FindNextElement("independentVar");
	 }

	string datafilename=T.tablename+".dat";

	if (T.nIVars==1)
		// ----------------------------------------------- 1D table

	{
		NTable += 1;
		tabledataEl = el->FindElement("tableData");
		T.nRows = el->FindElement("tableData")->GetNumDataLines();
		cout << "\tTableT Data rows: " << T.nRows << endl;
		T.nRows = tabledataEl->GetNumDataLines();
		cout << "\tTableT Data rows: " << T.nRows << endl;
		std::ofstream datafile(datafilename.c_str());
		datafile.seekp(0, std::ios::beg);
		datafile << "# " << T.tablename << endl;
		for (int i=0; i<=T.nRows-1; i++)
		{
			datafile << "             " << tabledataEl->GetDataLine(i) << endl;
		}
		T.nColumns = 2;
		T.ParValues.clear();
		datafile.close();
		//populate tableVec
		tableVec.push_back(T);
	}
	if (T.nIVars==2)
	{
		// ----------------------------------------------- 2D table
		NTable += 1;
		tabledataEl = el->FindElement("tableData");
		T.nRows = tabledataEl->GetNumDataLines();
		cout << "\tTableT Data rows: " << T.nRows << endl;
		std::ofstream datafile(datafilename.c_str());
		datafile.seekp(0, std::ios::beg);
		datafile << "# " << T.tablename << endl;
		// count parameters on the first row
		std::istringstream sdata;
		sdata.str("");
		vector<float> par;
		int npar=0;
//		sdata.str( el->FindElement("tableData")->GetDataLine(0) );
		sdata.str(tabledataEl->GetDataLine(0));
		float num = 0.;
		while ( sdata >> num )
		{
			par.push_back( num );
		}
		npar=par.size();
		T.nColumns = npar;
		T.ParValues = par;
					// write on file accordingly
		vector< vector<float> > m;
		vector<float> par1, row;
		for (int r=1; r<T.nRows; r++)
		{
			sdata.clear();
//			sdata.str( el->FindElement("tableData")->GetDataLine(r) );
			sdata.str(tabledataEl->GetDataLine(r));
			sdata.seekg(std::ios::beg);
			sdata >> num;
			par1.push_back(num);
			for (int p=1; p<=npar; p++)
			{
				sdata >> num;
				row.push_back(num);
			}
			m.push_back(row);
			row.clear();
		}
		for (unsigned int r=0; r<m.size(); r++)
		{
			datafile << "# " << T.iVars["row"] <<" = " << par1[r] << endl;
			for (unsigned int c=0; c<m[0].size(); c++)
			{
				datafile << par[c] << "  " << m[r][c] << endl;
			}
			datafile << endl;
			datafile << endl;
		}
		T.vParRow    = par;
		T.vParColumn = par1;
		T.m          = m;
		datafile.close();
		//populate tableVec
		tableVec.push_back(T);
	}
	if (T.nIVars == 3)
	{
		// ----------------------------------------------- 3D table

		string datafilename3d = T.tablename;
		cout << "TableT 3D table " <<el->GetAttributeValue("name")<<" found. "<< datafilename3d << endl;


		tabledataEl = el->FindElement("tableData");
		cout << "TableT tabledataEl " <<  " : " << tabledataEl << endl;
		while (tabledataEl)
		{
			NTable += 1;
			double key = tabledataEl->GetAttributeValueAsNumber("breakPoint");
			string skey = tabledataEl->GetAttributeValue("breakPoint");
			T.skeyname = skey;
			T.tablename = datafilename3d + skey;
			datafilename = datafilename3d + skey + ".dat";
			cout << "TableT datafilename :" << datafilename <<  endl;

			T.nRows = tabledataEl->GetNumDataLines();
			cout << "\tTableT Data rows: " << T.nRows << endl;
			std::ofstream datafile(datafilename.c_str());
			datafile.seekp(0, std::ios::beg);
			datafile << "# " << T.tablename << endl;
			// count parameters on the first row
			std::istringstream sdata;
			sdata.str("");
			vector<float> par;
			int npar = 0;
			sdata.str(tabledataEl->GetDataLine(0));
			float num = 0.;
			while (sdata >> num)
			{
				par.push_back(num);
			}
			npar = par.size();
			T.nColumns = npar;
			T.ParValues = par;
			// write on file accordingly
			vector< vector<float> > m;
			vector<float> par1, row;
			for (int r = 1; r<T.nRows; r++)
			{
				sdata.clear();
				sdata.str(tabledataEl->GetDataLine(r));
				sdata.seekg(std::ios::beg);
				sdata >> num;
				par1.push_back(num);
				for (int p = 1; p <= npar; p++)
				{
					sdata >> num;
					row.push_back(num);
				}
				m.push_back(row);
				row.clear();
			}
			for (unsigned int r = 0; r<m.size(); r++)
			{
				datafile << "# " << T.iVars["row"] << " = " << par1[r] << endl;
				for (unsigned int c = 0; c<m[0].size(); c++)
				{
					datafile << par[c] << "  " << m[r][c] << endl;
				}
				datafile << endl;
				datafile << endl;
			}
			T.vParRow = par;
			T.vParColumn = par1;
			T.m = m;
			datafile.close();

			tabledataEl = el->FindNextElement("tableData");
//			cout << "TableT (next)tabledataEl " << " : " << tabledataEl << endl;
//populate tableVec
			tableVec.push_back(T);
		}

	}

				//populate tableVec
//	tableVec.push_back(T);
				// clean T
	T.tablename="";
	T.funcname="";
	T.iVars.clear();//T.iVars.begin();
	T.nColumns=0;
	T.nIVars=0;
	T.nRows=0;
	T.ParValues.clear();
	T.m.clear();
	T.vParRow.clear();
	T.vParColumn.clear();
 } //end  of TableT

bool App::ScanFile(string root_tag)
{
/* scan xml config file searching for table */

	FGXMLParse script_file_parser;

	std::ifstream inputfile(inputfilename.c_str());
	readXML(inputfile, script_file_parser);
	document = script_file_parser.GetDocument();

	debug = 1;	// Also line 285 JSBSimthings/input_output/FGXMLElement.cpp
	NTable = 0;

	if (document->GetName() != root_tag ) 
	{
	   return false;
	}

	if ( debug )
	{
	   cout << "\nScanFile Everything in this document section" << endl;
	   funelement = document->FindElement("everything");
	}

	if ( root_tag != "aerodynamics" )
	{
	   // read aerodynamics data
	   cout << "\nScanFile Searching for aerodynamics" << endl;
	   aeroelement = document->FindElement("aerodynamics");
	}
	else
	{
	   cout << "\nScanFile Setting aeroelement = document." << endl;
	   aeroelement = document;
	}

	if ( aeroelement == 0 ) {
	   cout << "ScanFile aeroelement is empty" <<endl;
	   return false;
	}

	if ( debug ) 
	{
	   cout << "\nScanFile Everything in this aeroelement section" << endl;
	   funelement = aeroelement->FindElement("everything");
	}

	// read tables in function tags
	cout<< "\nScanFile --- Looking for tables in Function elements ---"<< endl << endl;
	funelement = aeroelement->FindElement("function");

	while(funelement)
	{
		cout << "ScanFile Current function element name: " << funelement->GetAttributeValue("name") << endl;
		xmlelement=funelement->FindElement();
		while (xmlelement)
		{
			cout<<"ScanFile	xmlelement: "<<xmlelement->GetName()<<endl;

			PrintT(xmlelement,0);
			xmlelement=funelement->FindNextElement();

		}
		funelement = aeroelement->FindNextElement("function");
	}

	// read tables in axis tags
	cout<< "\nScanFile --- Looking for tables in axis elements ---\n"<< endl;
	axelement = aeroelement->FindElement("axis");
	while (axelement)
	{
		cout << "ScanFile Current axis element name: " << axelement->GetAttributeValue("name") << endl;
		funelement=axelement->FindElement("function");
		while(funelement)
		{
			cout << "ScanFile	Current function element name: " << funelement->GetAttributeValue("name") << endl;
			xmlelement=funelement->FindElement();
			while (xmlelement)
			{
				PrintT(xmlelement,0);
				xmlelement=funelement->FindNextElement();
			}
			funelement = axelement->FindNextElement("function");
		};
		cout<<endl;
		axelement = aeroelement->FindNextElement("axis");
	}  // end if (axiselement)

	inputfile.close();
	return true;
}

