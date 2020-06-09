//
//  Utilities.hpp
//  Vulkan Sandbox
//
//  Created by Lars Andersson on 2020-06-09.
//  Copyright © 2020 Lars Andersson. All rights reserved.
//

#ifndef Utilities_h
#define Utilities_h

// Indicies (locations) of Queue Families (if they exist at all)
struct QueueFamilyIndices {
    int graphicsFamily = -1;       // Location of Graphics Queue Family
    
    bool isValid()
    {
        return graphicsFamily >= 0;
    };
};

#endif /* Utilities_h */
