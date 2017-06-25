// ======================================================================
//  Copyright (c) 2004-2005  Purdue Research Foundation
//  See the file "license.terms" for information on usage and
//  redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
// ======================================================================

#include "rappture.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include "Libstatmech.h"


#ifdef DEBUG
static int debug = 1;
#else 
static int debug = 0;
#endif

int main(int argc, char ** argv)
{

    RpLibrary* lib = NULL;

    Libstatmech__Fermion *p_fermion;

    std::string filePath = "";
    const char** xmltext = NULL;
    std::string s_title = "";
    double* fmin = NULL; double* fmax = NULL; double* d_n = NULL;
    int i;

    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " driver.xml" << std::endl;
    }

    filePath = std::string(argv[1]);

    if (debug) {
        std::cout << "filePath: " << filePath << std::endl;
    }

    // create a rappture library from the file filePath
    lib = rpLibrary(argv[1]);

    if (lib != NULL) {
            std::cout << "created Rappture Library successfully" << std::endl;
    }
    else {
        // cannot open file or out of memory
        std::cout << "FAILED creating Rappture Library" << std::endl;
        exit(1);
    }

    // get the xml that is stored in the rappture library lib
    rpXml(lib,xmltext);

    // get the min
    rpGetString(lib,"input.number(min).current",xmltext);
    std::string xmlstr = *xmltext;
    if (xmlstr.empty()) {
        std::cout << "lib->getString(input.number(xmin).current) returns null" << std::endl;
        delete lib;
        exit(1);
    }

    if(debug) {
        std::cout << "xml min :" << xmlstr << ": len=" << xmlstr.size() << std::endl;
    }

    // grab a double value from the xml
    rpGetDouble(lib,"input.number(min).current",fmin);

    if(debug) {
        std::cout << "min: " << fmin << std::endl;
    }

    // get the max
    rpGetDouble(lib,"input.(max).current",fmax);

    if(debug) {
        std::cout << "max: " << fmax << std::endl;
    }

    if(*fmin >= *fmax) {
      std::cout << "Max T must be greater than minimum T!" << std::endl;
      exit( EXIT_FAILURE );
    }

    // create the fermion
    const char** fermName = NULL;
    double* fermMass = NULL; double* fermMult = NULL; double* fermCharge = NULL;
    rpGetString(lib,"input.(name).current",fermName);
    rpGetDouble(lib,"input.(mass).current",fermMass);
    rpGetDouble(lib,"input.(multiplicity).current",fermMult);
    rpGetDouble(lib,"input.(charge).current",fermCharge);
    p_fermion =
     Libstatmech__Fermion__new(*fermName,*fermMass,(unsigned int) *fermMult,(int) *fermCharge);

    // get the number density
    rpGetDouble(lib,"input.(n).current",d_n);

    // label the graph with a title
    s_title.append(*fermName); s_title.append(" Chemical Potential");
    const char* stitle = s_title.c_str();
    rpPutString(lib,"output.curve(result).about.label",stitle,0);

    // label the xaxis
    rpPutString(lib,"output.curve(result).xaxis.label","Temperature",0);
    rpPutString(lib,"output.curve(result).xaxis.units","K",0);
    rpPutString(lib,"output.curve(result).xaxis.scale","log",0);

    // label the yaxis
    rpPutString(lib,"output.curve(result).yaxis.label","Chemical Potential / kT",0);

    // evaluate formula and generate results
    // science begins here

    double fx, fy;
    int npts = 100;
    std::stringstream myStr;

    for (i = 0; i<=npts; i++) {
        std::cout << i << std::endl;
        fx =
          pow(10.,log10(*fmin)+i*(log10(*fmax)-log10(*fmin) / npts));
        fy =
          Libstatmech__Fermion__computeChemicalPotential(p_fermion, fx, *d_n, NULL, NULL);
        myStr << fx << " " << fy << std::endl;
        const char* myChar = myStr.str().c_str();
        rpPutString(lib,"output.curve(result).component.xy",myChar,1);
        myStr.str("");
    }

    // write output to run file and signal
    rpResult(lib);
    rpFreeLibrary(&lib);

    Libstatmech__Fermion__free( p_fermion );

    delete lib;

    return 0;
}
