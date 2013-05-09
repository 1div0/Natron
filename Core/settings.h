#ifndef PROJECT_H
#define PROJECT_H
#include <vector>
#include <QtCore/qglobal.h>
#include "Core/metadata.h"

/*The current settings of Powiter in the preferences menu.*/
class Settings
{
public:
    Settings(){
        maxCacheMemoryPercent=0.5;
        maxPlayBackMemoryPercent = 0.14;
        maxDiskCache = 9000000000;
        byte_mode=1;
        stereo_mode=false;
    }
    
    double maxCacheMemoryPercent ;
    double maxPlayBackMemoryPercent;
    qint64 maxDiskCache ;
    // viewer related
    float byte_mode;
    bool stereo_mode;
};

#endif // PROJECT_H
