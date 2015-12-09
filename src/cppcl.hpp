/*
 * =====================================================================================
 *
 *       Filename:  cppcl.hpp
 *
 *    Description:  Headers und Krams fuer OpenCL
 *
 *        Version:  1.0
 *        Created:  10/12/2015 10:21:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Paul Thieme
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  cppcl_INC
#define  cppcl_INC

//  C++ headers
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

//  C headers
#include <cstdlib>

//  OpenCL C-bindings
#include <CL/cl.h>

//  enable exceptions for C++ binding
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#endif   /* ----- #ifndef cppcl_INC  ----- */
