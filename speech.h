#include <fstream>
#include "wav.h"
#include <map>
#include "itensor/all.h"
#include "mllib/data.h"

using namespace itensor;
using std::vector;
using std::array;

namespace mllib {

using SpeechData = Data<Real,2ul>;

void
haarRescale(vector<double> & v,
            int Nhaar)
    {
    int size = v.size();
    for(int pass = 1; pass <= Nhaar; ++pass)
        {
        size = size/2;
        if(size < 2)
            {
            printfln("Error: new size would be %d",size);
            exit(0);
            }
        auto p = 0;
        for(auto i : range(size))
            {
            v[i] = (v[p]+v[p+1])/Sqrt2;
            p += 2;
            }
        }
    v.resize(size);
    }

std::vector<SpeechData> inline
readSpeech(string datadir,
           DataType type,
           Args const& args = Args::global())
    {
    auto align_thresh = args.getReal("AlignThreshold",0.);
    auto NT = args.getInt("NT",50000);
    auto Nhaar = args.getInt("Nhaar",0);
    auto normalize = args.getBool("Normalize",false);
    printfln("Reading Speech Commands %s data",type==Train?"training":"testing");
    printfln("  %s normalize the data",normalize?"Will":"Will not");
    printfln("  Doing %d Haar rescalings of data",Nhaar);
    printfln("  Using an alignment threshold of %.5f",align_thresh);


    auto tset = vector<SpeechData>{};
    string lfname = datadir + "/labels.txt";
    std::ifstream lf(lfname);
    if(!lf.is_open()) Error("Error opening training info file.");

    println("  Reading files");
    vector<string> labels;
    string label;
    while(lf >> label)
        {
        labels.push_back(label);
        }
    lf.close();

    auto mins = array<Real,SpeechData::NL>{};
    for(auto& m : mins) m = +1.234E5;
    auto maxs = array<Real,SpeechData::NL>{};
    for(auto& m : maxs) m = -1.234E5;

    string ending;
    if(type == Train)
        {
        ending = "_train.txt";
        }
    else if(type == Test)
        {
        ending = "_test.txt";
        }

    auto Ntot = 0;
    auto labelc = 0;
    for(auto lname : labels)
        {
        std::ifstream cfile(datadir+"/"+lname+ending);
        string fname;
        while(cfile >> fname)
            {
            tset.push_back(SpeechData());
            auto& t = tset.back();
            Ntot += 1;
            t.n = Ntot;
            t.label = labelc;
            t.type = type;
            t.name = fname;

            t.data = readWAV(datadir+"/"+lname+"/"+fname);
                //auto nrm2_0 = 0.;
                //for(auto& r : t.data) nrm2_0 += r*r;
                //printfln("Norm before rescaling = %.14f",sqrt(nrm2_0));
            if(Nhaar > 0) haarRescale(t.data,Nhaar);
            if(normalize)
                {
                //Subtract average
                auto avg = 0.;
                for(auto& r : t.data) avg += r;
                avg /= t.data.size();
                for(auto& r : t.data) r -= avg;

                //Normalize (make variance = 1)
                auto nrm2 = 0.;
                for(auto& r : t.data) nrm2 += r*r;
                //printfln("Norm after rescaling = %.14f",sqrt(nrm2));
                //PAUSE
                if(nrm2 < 1E-14)
                    {
                    printfln("Warning: encountered nrm2 = %.2E",nrm2);
                    }
                mins.at(t.label) = std::min(mins[t.label],sqrt(nrm2));
                maxs.at(t.label) = std::max(maxs[t.label],sqrt(nrm2));
                auto nfac = 1./sqrt(nrm2);
                for(auto& r : t.data) r *= nfac;
                }
            if(align_thresh > 0.)
                {
                auto old = t.data;
                stdx::fill(t.data,0.);
                int size = t.data.size();
                int n = 0;
                while(n < size && fabs(old[n]) < align_thresh)
                    {
                    ++n;
                    }
                int i = 0;
                for(int j = n; j < size; ++j)
                    {
                    t.data[i] = old[j];
                    ++i;
                    }
                }
            }
        labelc += 1;
        }
    lf.close();

    std::random_shuffle(tset.begin(),tset.end());
    if(Ntot >= NT) tset.resize(NT);

    if(normalize)
        {
        println("  Norm ranges:");
        for(auto l : range(SpeechData::NL))
            {
            printfln("    %d [%.12f,%.12f]",l,mins[l],maxs[l]);
            }
        }

    printfln("  %d label types",labels.size());

    return tset;
    }

} //namespace mllib
