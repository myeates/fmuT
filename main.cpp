// ======================================================================
//  Copyright (c) 2004-2005  Purdue Research Foundation
//  See the file "license.terms" for information on usage and
//  redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
// ======================================================================

#include "rappture.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <Libstatmech.h>


#ifdef DEBUG
static int debug = 1;
#else 
static int debug = 0;
#endif

int main(int argc, char * argv[])
{

    RpLibrary* lib = NULL;

    Libstatmech__Fermion *p_fermion;

    std::string filePath = "";
    std::string xmltext = "";
    std::string s_title = "";
    double fmin, fmax, d_n;
    std::string strFormula = "";
    int i;

    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " driver.xml" << std::endl;
    }

    filePath = std::string(argv[1]);

    if (debug) {
        std::cout << "filePath: " << filePath << std::endl;
    }

    // create a rappture library from the file filePath
    lib = new RpLibrary(filePath);

    if (lib) {
        if(debug) {
            std::cout << "created Rappture Library successfully" << std::endl;
        }
    }
    else {
        // cannot open file or out of memory
        std::cout << "FAILED creating Rappture Library" << std::endl;
        exit(1);
    }

    // get the xml that is stored in the rappture library lib
    xmltext = lib->xml();

    if(! (xmltext.empty()) ) {
        if(debug) {
        //printf("XML file content:\n");
        //printf("%s\n", xmltext);
    }
    }
    else {
        printf("lib->xml() failed\n");
        delete lib;
        exit(1);
    }

    // get the min
    xmltext = lib->getString("input.number(min).current");

    if ( xmltext.empty() ) {
        std::cout << "lib->getString(input.number(xmin).current) returns null" << std::endl;
        delete lib;
        exit(1);
    }

    if(debug) {
        std::cout << "xml min :" << xmltext << ": len=" << xmltext.size() << std::endl;
    }

    // grab a double value from the xml
    fmin = lib->getDouble("input.number(min).current");

    if(debug) {
        std::cout << "min: " << fmin << std::endl;
    }

    // get the max
    fmax = lib->getDouble("input.(max).current");
    if(debug) {
        std::cout << "max: " << fmax << std::endl;
    }

    if( fmin >= fmax )
    {
      std::cout << "Max T must be greater than minimum T!" << std::endl;
      exit( EXIT_FAILURE );
    }

    // create the fermion
    p_fermion =
      Libstatmech__Fermion__new(
        (lib->getString("input.(name).current")).c_str(),
        lib->getDouble("input.(mass).current"),
        (unsigned int) lib->getDouble("input.(multiplicity).current"),
        (int) lib->getDouble("input.(charge).current")
      );

    // get the number density
    d_n = lib->getDouble( "input.(n).current" );

    // label the graph with a title
    s_title.append( (lib->getString("input.(name).current")).c_str() );
    s_title.append( " Chemical Potential" );
    lib->put(
      "output.curve(result).about.label",
      s_title.c_str(),"",RPLIB_OVERWRITE
    );

    // label the xaxis
    lib->put(
      "output.curve(result).xaxis.label",
      "Temperature","",RPLIB_OVERWRITE
    );
    lib->put(
      "output.curve(result).xaxis.units",
      "K","",RPLIB_OVERWRITE
    );
    lib->put(
      "output.curve(result).xaxis.scale",
      "log","",RPLIB_OVERWRITE
    );

    // label the yaxis
    lib->put(
      "output.curve(result).yaxis.label",
      "Chemical Potential / kT","",RPLIB_OVERWRITE
    );

    // evaluate formula and generate results
    // science begins here

    double fx, fy;
    int npts = 100;
    std::stringstream myStr;

    for (i = 0; i<=npts; i++) {
        std::cout << i << std::endl;
        fx =
          pow(
            10.,
            log10( fmin ) + i * ( log10( fmax ) - log10( fmin ) ) / npts
          );
        fy =
          Libstatmech__Fermion__computeChemicalPotential(
            p_fermion,
            fx,
            d_n,
            NULL,
            NULL
          );
        myStr << fx << " " << fy << std::endl;
        lib->put(
          "output.curve(result).component.xy",
          myStr.str(),
          "",
          RPLIB_APPEND
        );
        myStr.str("");
    }

    // write output to run file and signal
    lib->result();

    Libstatmech__Fermion__free( p_fermion );

    delete lib;

    return 0;
}
