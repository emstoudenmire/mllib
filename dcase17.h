#include <fstream>
#include "wav.h"
#include <map>
#include "itensor/all.h"
#include "mllib/data.h"

using namespace itensor;
using std::vector;
using std::array;

namespace mllib {

using DCaseData = Data<Real,15ul>;

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

std::vector<DCaseData> inline
readDCASE17(string datadir,
            DataType type,
            Args const& args = Args::global())
    {
    auto normalize = args.getBool("Normalize",false);
    auto NT = args.getInt("NT",50000);
    auto Nhaar = args.getInt("Nhaar",0);
    auto index_start = args.getInt("IndexStart",0);
    printfln("Reading DCASE17 %s data",type==Train?"training":"testing");
    printfln("  %s normalize the data",normalize?"Will":"Will not");
    printfln("  Doing %d Haar rescalings of data",Nhaar);

    datadir = datadir + "/TUT-acoustic-scenes-2017-development/";

    auto tset = vector<DCaseData>{};
    string lfname;
    if(type == Train) 
        {
        lfname = datadir+"evaluation_setup/fold1_train.txt";
        tset.reserve(3510);
        }
    else if(type == Test) 
        {
        lfname = datadir+"evaluation_setup/fold1_evaluate.txt";
        tset.reserve(1170);
        }

    std::ifstream lf(lfname);
    if(!lf.is_open()) Error("Error opening training info file.");

    std::map<string,int> counts;
    std::map<string,int> labels;
    struct NmLb 
        { 
        string fname; 
        string label; 
        NmLb(string n, string l) : fname(n),label(l) { }
        };
    vector<NmLb> nmlb;

    println("  Reading files");
    auto Ntot = 0;
    auto labelc = index_start;
    string fname,label;
    while(lf >> fname >> label)
        {
        nmlb.emplace_back(fname,label);
        }
    auto mins = array<Real,15>{};
    for(auto& m : mins) m = +1.234E5;
    auto maxs = array<Real,15>{};
    for(auto& m : maxs) m = -1.234E5;
    for(auto& nl : nmlb)
        {
        Ntot += 1;
        tset.push_back(DCaseData());
        auto& t = tset.back();
        if(labels.count(nl.label)==0) labels[nl.label] = labelc++;
        t.n = Ntot;
        t.label = labels[nl.label];
        t.type = type;

        t.data = readWAV(datadir+nl.fname);
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

        if(Ntot%100 == 0)
            {
            printf(" %d/%d",Ntot,nmlb.size());
            }

        if(counts.count(nl.label)==0)
            {
            counts[nl.label] = 1;
            }
        else
            {
            counts[nl.label] += 1;
            }
        if(Ntot >= NT) break;
        }
    lf.close();
    printfln(" %d/%d",Ntot,nmlb.size());

    if(normalize)
        {
        println("  Norm ranges:");
        for(auto l : range(15))
            {
            printfln("    %d [%.12f,%.12f]",l,mins[l],maxs[l]);
            }
        }

    printfln("  %d label types",labels.size());

    println("  Mapping from integers -> labels:");
    for(auto it = labels.begin(); it != labels.end(); ++it) 
        {
        printfln("    %02d %d",it->second,it->first);
        }
    println("  Individual label counts:");
    for(auto it = counts.begin(); it != counts.end(); ++it) 
        {
        printfln("    %d of type %s",it->second,it->first);
        }

    return tset;
    }

} //namespace mllib
