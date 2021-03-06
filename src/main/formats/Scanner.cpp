/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "Scanner.h"
#include "Root.h"

bool VGMScanner::Init() {
    // if (!UseExtension())
    //	return false;
    return true;
}

void VGMScanner::InitiateScan(RawFile *file, void *offset) {
    pRoot->UI_SetScanInfo();
    this->Scan(file, offset);
}

// void VGMScanner::Scan(RawFile* file)
//{
//	return;
//}
