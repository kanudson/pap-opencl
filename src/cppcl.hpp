/*
 * =====================================================================================
 *
 *       Filename:  cppcl.hpp
 *
 *    Description:  
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

#include <string>
#include <vector>

#include <cstdlib>

#include <CL/cl.h>

namespace ocl
{
//  helper functions
std::string get_clPlatformInfo(cl_platform_id platform, cl_platform_info info);

cl_uint getPlatformCount();
std::vector<cl_platform_id> getPlatformList();

void printClError(cl_uint error, const char* msg);
void printClError(cl_uint error, std::string msg);
}   //  namespace ocl

#endif   /* ----- #ifndef cppcl_INC  ----- */

