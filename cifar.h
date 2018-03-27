#ifndef __MLLIB_CIFAR_H_
#define __MLLIB_CIFAR_H_
#include "itensor/all.h"
#include "mllib/data.h"

namespace mllib {

using itensor::Matrix;
using itensor::Args;
using itensor::range;
using itensor::range1;
using itensor::format;
using itensor::println;
using itensor::printfln;
using std::array;

//
// CIFAR data sets: http://www.cs.toronto.edu/~kriz/cifar.html
//

struct RGB
    {
    double r = 0;
    double g = 0;
    double b = 0;

    RGB() { }
    };

using CFData = Data<RGB,10>;
 
std::vector<CFData>
readCIFAR(std::string dirname, 
          DataType type = Train,
          Args const& args = Args::global())
	{
    using Byte = unsigned char;
    auto normalize = args.getBool("Normalize",false);
    auto NT = args.getInt("NT",500000);

    int file_nimg = 10000;

    auto imgs = std::vector<CFData>{};

    int nrows = 32;
    int ncols = 32;
    int N = nrows*ncols;

    auto count = array<int,10>{};

    auto colors = array<Matrix,3>{};
    for(auto c : range(3)) colors[c] = Matrix(nrows,ncols);

    auto nfiles = (type==Train) ? 5 : 1;

    auto counts = array<int,CFData::NL>{};
    for(auto fnum : range1(nfiles))
        {
        std::string filename;
        if(type==Train)
            {
            filename = format("%s/data_batch_%d.bin",dirname,fnum);
            }
        else
            {
            filename = format("%d/test_batch.bin",dirname);
            }
        println("Reading file ",filename);
        std::ifstream file(filename,std::ios::binary);
        if(not file.is_open()) throw std::runtime_error(format("Could not open file %s",filename));

        auto readByte = [&file]()
            {
            Byte b = 0;
            file.read((char*) &b, sizeof(b));
            return b;
            };

        for(auto n : range(file_nimg))
            {
            int label = readByte();
            if(label < 0 || label > 9)
                {
                println("label = ",label);
                PAUSE
                }

            auto dat = CFData(n,type,label,nrows*ncols);

            if(dat.label < 0)
                {
                Print(label);
                Print(dat.label);
                PAUSE
                }

            for(auto c : range(3))
               {
               for(auto y : range(nrows))
               for(auto x : range(ncols))
                   {
                   colors[c](x,y) = readByte();
                   }
               }

            for(auto y : range(nrows))
            for(auto x : range(ncols))
               {
               dat[y+nrows*x].r = colors[0](x,y);
               dat[y+nrows*x].g = colors[1](x,y);
               dat[y+nrows*x].b = colors[2](x,y);
               }

            if(counts[label] >= NT) continue;
            imgs.emplace_back(std::move(dat));
            counts[label] += 1;
            }
        }
    Print(imgs.size());
    println("Number of images per label type:");
    for(auto n : range(count))
        {
        printfln("%d %d",n,counts[n]);
        }

    if(normalize)
        {
        println("Normalizing images");
        for(auto& img : imgs)
            {
            auto avg_r = 0.;
            auto avg_g = 0.;
            auto avg_b = 0.;
            //Rescale to [0,1] and compute average
            for(auto j : range1(N))
                {
                img(j).r /= 255.;
                img(j).g /= 255.;
                img(j).b /= 255.;

                avg_r += img(j).r;
                avg_g += img(j).g;
                avg_b += img(j).b;
                }
            avg_r /= N;
            avg_g /= N;
            avg_b /= N;
            //Subtract averages
            //auto max_r = -1000.;
            //auto min_r = 10000.;
            for(auto j : range1(N))
                {
                img(j).r -= avg_r;
                img(j).g -= avg_g;
                img(j).b -= avg_b;
                //max_r = std::max(max_r,img(j).r);
                //min_r = std::min(min_r,img(j).r);
                }
            }
        }

    for(auto& img : imgs)
        {
        if(img.label < 0 || img.label > 9)
            {
            println("in readCIFAR, img.label = ",img.label);
            PAUSE
            }
        }

    return imgs;
    }

} //namespace mllib

#endif
