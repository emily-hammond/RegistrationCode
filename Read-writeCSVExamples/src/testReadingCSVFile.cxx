/*
 * Emily Hammond
 * 5/29/14
 *
 * The purpose of this code is to demonstrate/debug the reading/writing of CSV files in C++.
 *
 *NOTE: The csv file must be separated by spaces, not commas to allow for accurate comparison of image names********
 *
 */

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>

int main( int argc, char * argv[] )
{
    std::string filename = argv[1];
    std::string imageOfInterest = argv[2];

    std::fstream file;
    file.open( filename.c_str() );

    std::string line = "";
    char image[255];
    float corina [3];
    float heart [3];
    float aorta [3];

    bool header = true;

    do
    {
        std::getline(file,line);

        if(header)
        {
            std::cout << line << std::endl << std::endl;
        }
        else
        {
            sscanf(line.c_str(), "%s %f %f %f %f %f %f %f %f %f", image, &corina[0], &corina[1], &corina[2], &heart[0], &heart[1], &heart[2], &aorta[0], &aorta[1], &aorta[2]);

//            std::cout << line << std::endl;
//            std::cout << image << std::endl;
//            std::cout << corina[0] << " " << corina[1] << " " << corina[2] << std::endl;
//            std::cout << heart[0] << " " << heart[1] << " " << heart[2] << std::endl;
//            std::cout << aorta[0] << " " << aorta[1] << " " << aorta[2] << std::endl << std::endl;
        }

        header = false;
    }while( file.good() );

    file.close();

    file.open( filename.c_str() );
    do
    {
        std::getline(file,line);
        sscanf(line.c_str(), "%s", image, &corina[0], &corina[1], &corina[2], &heart[0], &heart[1], &heart[2], &aorta[0], &aorta[1], &aorta[2]);

        std::cout << image << std::endl;
        std::cout << imageOfInterest << std::endl;
        std::cout << strcmp(image,imageOfInterest.c_str()) << std::endl;
        if( strcmp(image, imageOfInterest.c_str()) == 0 )
        {
            sscanf(line.c_str(), "%s %f %f %f %f %f %f %f %f %f", image, &corina[0], &corina[1], &corina[2], &heart[0], &heart[1], &heart[2], &aorta[0], &aorta[1], &aorta[2]);

            std::cout << image << std::endl;
            std::cout << corina[0] << " " << corina[1] << " " << corina[2] << std::endl;
            std::cout << heart[0] << " " << heart[1] << " " << heart[2] << std::endl;
            std::cout << aorta[0] << " " << aorta[1] << " " << aorta[2] << std::endl << std::endl;
        }
    }while( file.good() );

    file.close();

    return 0;
}
