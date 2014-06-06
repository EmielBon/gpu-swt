//
//  main.cpp
//  OGLTest
//
//  Created by Emiel Bon on 29-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "RenderWindow.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char * argv[])
{
    //try
   // {
        RenderWindow window(512, 512, "SWT Render Window");
        window.Show();
    //}
    //catch (const Exception &e)
    //{
    //    std::cerr << "ERROR: " << e.what() << std::endl;
    //    return EXIT_FAILURE;
   // }
    
    return EXIT_SUCCESS;
}