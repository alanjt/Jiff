#pragma once
#include "input_output/FGXMLelement.h"
#include <string>
#include <map>
#include <vector>

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
class ElementJiff : public Element{

public:

	ElementJiff(const std::string& nm);

	~ElementJiff(void);

  /** Prints the element.
  *   Prints this element and calls the Print routine for child elements.
  *   @param d The tab level. A level corresponds to a single space. */
void ElementJiff::PrintJiff(unsigned int level=0);

private:
  std::string name;
  std::map <std::string, std::string> attributes;
  std::vector <std::string> data_lines;
  std::vector <Element_ptr> children;
  Element *parent;
  unsigned int element_index;
  std::string file_name;
  int line_number;
  typedef std::map <std::string, std::map <std::string, double> > tMapConvert;
  static tMapConvert convert;
  static bool converterIsInitialized;
};

} // namespace JSBSim


