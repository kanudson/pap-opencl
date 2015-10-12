/*
 * =====================================================================================
 *
 *       Filename:  cppcl.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/12/2015 10:32:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Paul Thieme
 *   Organization:  
 *
 * =====================================================================================
 */

#include "cppcl.hpp"

namespace ocl
{

/*
 * cl_int clGetPlatformInfo (cl_platform_id platform,
 *                           cl_platform_info param_name,
 *                           size_t param_value_size,
 *                           void * param_value,
 *                           size_t * param_value_size_ret)
 */
std::string get_clPlatformInfo(cl_platform_id platform, cl_platform_info info)
{
    cl_int err = 0;
    std::size_t length;

    err = clGetPlatformInfo(platform, info, 0, nullptr, &length);
    std::vector<char> str(length);
    err = clGetPlatformInfo(platform, info, length, str.data(), nullptr);

    return std::string(str.data());
}

//
cl_uint getPlatformCount()
{
    cl_uint number = 0;
    cl_int err = clGetPlatformIDs(0, nullptr, &number);
    if (err != CL_SUCCESS)
    {
        throw "clGetPlatformIDs throws";
    }
    return number;
}

//  
std::vector<cl_platform_id> getPlatformList()
{
    auto number = getPlatformCount();
    std::vector<cl_platform_id> platformList(number);
    clGetPlatformIDs(number, platformList.data(), nullptr);
    return platformList;
}

}   //  namespace ocl

