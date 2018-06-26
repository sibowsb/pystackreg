#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <iterator>

#include "TurboReg.h"
#include "TurboRegImage.h"
#include "TurboRegMask.h"
#include "TurboRegPointHandler.h"
#include "TurboRegTransform.h"
#include "matrix.h"

using namespace std;

std::vector<uint16_t> readFile(const char* filename)
{
    /*// open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<uint16_t> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
               std::istream_iterator<uint16_t>(file),
               std::istream_iterator<uint16_t>());

    return vec;*/

   std::ifstream is;
   std::vector<uint16_t> rawfilebuffer;

   is.open(filename, std::ios::binary);
   is.seekg(0, std::ios::end);
   size_t filesize=is.tellg();
   is.seekg(0, std::ios::beg);

   rawfilebuffer.resize(filesize/sizeof(uint16_t));

   is.read((char *)rawfilebuffer.data(), filesize);

   return rawfilebuffer;

}

int main(int argc, char **argv)
{
    //int width = 128;
    //int height = 256;

	//swapped
	int width = 256;
	int height = 128;



    std::vector<uint16_t> int_imgdata_ref = readFile("/media/storage/eric/data/180614_Rut1_Papain/Use These Files/pygreg_test_ref.bin");
    std::vector<uint16_t> int_imgdata_mov = readFile("/media/storage/eric/data/180614_Rut1_Papain/Use These Files/pygreg_test_mov.bin");

    std::vector<double> imgdata_ref(int_imgdata_ref.begin(), int_imgdata_ref.end());
    std::vector<double> imgdata_mov(int_imgdata_mov.begin(), int_imgdata_mov.end());

    double *pDataRef= &imgdata_ref[0];
    double *pDataMov= &imgdata_mov[0];

    TurboRegImage refImg(pDataRef, width, height, RIGID_BODY, true);
    TurboRegImage movImg(pDataMov, width, height, RIGID_BODY, false);

    TurboRegPointHandler refPH(refImg);
    TurboRegPointHandler movPH(movImg);

    TurboRegMask refMsk(refImg);
    TurboRegMask movMsk(movImg);

    refMsk.clearMask();
    movMsk.clearMask();

    int pyramidDepth = getPyramidDepth(
    		movImg.getWidth(), movImg.getHeight(),
			refImg.getWidth(), refImg.getHeight()
			);
    refImg.setPyramidDepth(pyramidDepth);
    refMsk.setPyramidDepth(pyramidDepth);
    movImg.setPyramidDepth(pyramidDepth);
    movMsk.setPyramidDepth(pyramidDepth);
    
    refImg.init();
    refMsk.init();
    movImg.init();
    movMsk.init();

  
    TurboRegTransform tform(movImg, movMsk, movPH, refImg, refMsk, refPH, RIGID_BODY, false);
    
    tform.doRegistration();
    //matrix<double> tmat = tform.get
    std::vector<double> imgout = tform.doFinalTransform(width, height);

    tform.printPoints();

	return 0;
}
