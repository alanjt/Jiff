#include "ElementJiff.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "input_output/FGXMLElement.h"
#include "input_output/string_utilities.h"
#include "FGJSBBase.h"

using namespace std;

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


ElementJiff::ElementJiff(const string& nm)
{
  name   = nm;
  parent = 0L;
  element_index = 0;
  line_number = -1;
}

ElementJiff::~ElementJiff(void)
{
	for (unsigned int i = 0; i < children.size(); ++i)
    children[i]->SetParent(0);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void ElementJiff::PrintJiff(unsigned int level)
{
  unsigned int i, spaces;

  level+=2;
  for (spaces=0; spaces<=level; spaces++) cout << " "; // format output
  cout << "Element Name: " << name;

  map<string, string>::iterator it;
  for (it = attributes.begin(); it != attributes.end(); ++it)
    cout << "  " << it->first << " = " << it->second;

  cout << endl;
  for (i=0; i<data_lines.size(); i++) {
    for (spaces=0; spaces<=level; spaces++) cout << " "; // format output
    cout << data_lines[i] << endl;
  }
  for (i=0; i<children.size(); i++) {
    children[i]->Print(level);
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}