#ifndef __MLLIB_WAV_H_
#define __MLLIB_WAV_H_
#include <sndfile.h>
#include "itensor/all.h"

namespace mllib {

std::vector<double> inline
readWAV(std::string fname)
    {
    SF_INFO info;
    SNDFILE* sf = sf_open(fname.c_str(),SFM_READ,&info);
    if(sf == NULL)
        {
        std::cout << "readWAV: failed to open file " << fname << std::endl;
        exit(-1);
        }
    auto nframes = info.frames;
    auto nchannels = info.channels;
    auto num_items = nframes*nchannels;
    auto buffer = std::vector<double>(num_items);
    auto num_frames_read = sf_readf_double(sf,buffer.data(),info.frames);
    sf_close(sf);
    if(nframes != num_frames_read)
        {
        std::cout << "readWAV: error, did not finish reading file" << std::endl;
        exit(-1);
        }
    
    //auto duration_in_s = (buffer.size()*1.)/(info.samplerate*info.channels);
    //Print(duration_in_s);

    std::vector<double> wav;

    if(nchannels == 1)
        {
        wav.swap(buffer);
        }
    else if(nchannels == 2)
        {
        wav = std::vector<double>(nframes,0.);
        assert(buffer.size() == 2*wav.size());
        for(decltype(wav.size()) f = 0; f < wav.size(); ++f)
            {
            wav[f] = (buffer[f]+buffer[f+1])/2.;
            }
        }

    return wav;
    }

//std::vector<double> inline
//readWAV(std::string fname)
//    {
//    std::ifstream file(fname,std::ios::binary);
//    if(!file.is_open())
//      {
//      std::cout << "Error opening audio file "+fname << std::endl;
//      exit(0);
//      }
//
//    //
//    // Read header information just to skip
//    // to actual audio data
//    //
//    auto id = readString(file,4);
//    Print(id);
//    assert(id == "RIFF");
//    auto size = readUInt(file);
//    Print(size);
//    auto type = readString(file,4);
//    Print(type);
//    assert(type == "WAVE");
//    auto id_fmt = readString(file,4);
//    Print(id_fmt);
//    auto chunksize = readUInt(file);
//    Print(chunksize);
//    auto format_tag = readShort(file);
//    Print(format_tag);
//    assert(format_tag == (short)1);
//    auto nchannel = readShort(file);
//    Print(nchannel);
//    auto samplerate = readUInt(file);
//    Print(samplerate);
//    auto byterate = readUInt(file);
//    Print(byterate);
//    auto blockalign = readShort(file);
//    Print(blockalign);
//    auto bitspersample = readShort(file);
//    Print(bitspersample);
//    auto id_data = readString(file,4);
//    Print(id_data);
//    assert(id_data == "data");
//    auto nbytes_data = readUInt(file);
//    Print(nbytes_data);
//
//    //Initialize bytesread to size of header info
//    auto bytesread = 4*4*sizeof(char)
//                    +5*sizeof(unsigned int)
//                    +4*sizeof(short);
//
//    int bytespersample = bitspersample/8;
//
//    size_t nsamples = (nbytes_data/bytespersample)/nchannel;
//
//    //auto duration_in_seconds = nsamples*1./samplerate;
//    //Print(duration_in_seconds);
//
//    unsigned int maxrange = 0;
//    if(bytespersample == 1) maxrange = 128;
//    else if(bytespersample == 2) maxrange = 32768;
//    else if(bytespersample == 3) maxrange = 8388608;
//    else if(bytespersample == 4) maxrange = 2147483648;
//    else
//        {
//        std::cout << "Error: not expecting bytespersample = "
//                  << bytespersample << std::endl;
//        }
//    Print(maxrange);
//
//    auto wav = std::vector<double>(nsamples);
//
//    if(nchannel == (short)1)
//        {
//        auto maxel = 0.0;
//        unsigned int buffer = 0;
//        for(decltype(nsamples) b = 0; b < nsamples; ++b)
//            {
//            file.read((char*)&buffer, bytespersample);
//            auto r = static_cast<double>(buffer)/maxrange;
//            if(r > maxel) maxel = r;
//            wav[b] = r;
//            }
//        if(maxel > 1.5)
//            {
//            std::cout << "Error: max element was " << maxel << std::endl;
//            exit(0);
//            }
//        }
//    else
//        {
//        //Two-channel audio case
//        auto maxel = 0.0;
//        //unsigned int Lbuffer = 0, Rbuffer = 0;
//        uint64_t Lbuffer = 0, Rbuffer = 0;
//        for(decltype(wav.size()) b = 0; b < wav.size(); ++b)
//            {
//            file.read((char*)&Lbuffer, bytespersample);
//            file.read((char*)&Rbuffer, bytespersample);
//            Lbuffer = Lbuffer >> 8;
//            Rbuffer = Rbuffer >> 8;
//            auto Lr = static_cast<double>(Lbuffer)/maxrange;
//            auto Rr = static_cast<double>(Rbuffer)/maxrange;
//                Print(Lbuffer);
//                Print(Lr);
//                PAUSE
//            if(Lr > maxel) maxel = Lr;
//            if(Rr > maxel) maxel = Rr;
//            wav[b] = (Lr+Rr)/2.;
//
//            bytesread += 2*bytespersample;
//            }
//        if(maxel > 1.5)
//            {
//            std::cout << "Error: max element was " << maxel << std::endl;
//            exit(0);
//            }
//        }
//    file.close();
//
//    //Definition of "size" is size of header plus data in bytes,
//    //not counting "RIFF" + size itself (8 initial bytes)
//    assert(8+size == bytesread);
//    //Print(bytesread);
//    //Print(8+size);
//
//    exit(0);
//
//    return wav;
//    }

} //namespace mllib

#endif
