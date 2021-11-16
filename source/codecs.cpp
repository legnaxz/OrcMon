#include "codecs.h"

#include "memory"
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <tcp_client.h>
#include <log4cxx/logger.h>

static std::string ffmpeg_path = "/home/yes/OrcMon/ffmpeg -i ";
static std::string map_channel_0 = "-map_channel 0.0.0 ";
static std::string map_channel_1 = "-map_channel 0.0.1 ";

namespace orc
{
    class Codecs::Private
    {
        typedef struct WAV_HEADER
        {
            /* RIFF Chunk Descriptor */
            uint8_t RIFF[4];    // RIFF Header Magic header
            uint32_t ChunkSize; // RIFF Chunk Size
            uint8_t WAVE[4];    // WAVE Header
            /* "fmt" sub-chunk */
            uint8_t fmt[4];         // FMT header
            uint32_t Subchunk1Size; // Size of the fmt chunk
            uint16_t AudioFormat;   // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
            uint16_t NumOfChan;     // Number of channels 1=Mono 2=Sterio
            uint32_t SamplesPerSec; // Sampling Frequency in Hz
            uint32_t bytesPerSec;   // bytes per second
            uint16_t blockAlign;    // 2=16-bit mono, 4=16-bit stereo
            uint16_t bitsPerSample; // Number of bits per sample
            /* "data" sub-chunk */
            uint8_t Subchunk2ID[4]; // "data"  string
            uint32_t Subchunk2Size; // Sampled data length
        } wav_hdr;
      
        log4cxx::LoggerPtr Logger_ = log4cxx::Logger::getLogger("Codecs");

    public:
        Private(Codecs *codecs)
            : codecs_(codecs)
        {
        }
        ~Private()
        {
        }

    public:
      
        inline std::string removeExtension( const std::string filename )
        {
            size_t lastdot = filename.find_last_of(".");
            if ( lastdot == std::string::npos ) {
                return filename;
            }
            return filename.substr( 0, lastdot );
        }
     
        inline int wavSplit( const std::string path, const std::string filename )
        {
            int total_len = 0;
            int header_size = sizeof( wav_hdr_ );
            int wait_command_finish = 0;
            uint32_t bytes_size;
            
            std::string file_path = path + filename;
            std::string output_path = path + "intermediate/";
            
            std::ifstream file( file_path.c_str(), 
                                std::ifstream::in | std::ifstream::binary );

            if ( file.is_open() )
            {
                file.seekg( 0, file.end );
                total_len = ( int )file.tellg();
                file.seekg( 0, file.beg );

                unsigned char *header = new unsigned char[header_size];

                file.read( ( char * )&wav_hdr_, header_size );

                if ( 0 == strncmp( "RIFF", ( char * ) wav_hdr_.RIFF, 4 ) )
                {
                    LOG4CXX_DEBUG( Logger_, "Is Wav File." );

                    if ( 0 == strncmp( "SIGN", ( char * ) wav_hdr_.fmt, 4 ) )
                    {
                        LOG4CXX_DEBUG( Logger_, "Is G729 Format." );
                    }
                    else if ( 0 == strncmp ( "fmt ", ( char * ) wav_hdr_.fmt, 4 ) )
                    {
                        LOG4CXX_DEBUG( Logger_, "Is Wav Format." );
                        std::string output_file_ch0 = removeExtension( filename ) + "_ch0.wav";
                        std::string output_file_ch1 = removeExtension( filename ) + "_ch1.wav";

                        std::string split_command = ffmpeg_path + file_path + " " +
                                                        map_channel_0 + output_path + output_file_ch0 + " " +
                                                        map_channel_1 + output_path + output_file_ch1;
                        std::cout << split_command << std::endl;
                        wait_command_finish = system( split_command.c_str() );
                    }
                    else
                    {
                        LOG4CXX_DEBUG( Logger_, "Is Invalid Format." );
                    }
                }
                else
                {
                   
                    LOG4CXX_DEBUG( Logger_, "Is PCM File." );
                    std::string output_file_ch0 = removeExtension( filename ) + "_ch0.wav";
                    std::string output_file_ch1 = removeExtension( filename ) + "_ch1.wav";

                    std::string split_command = ffmpeg_path + file_path + " " +
                                                    map_channel_0 + output_file_ch0 + " " +
                                                    map_channel_1 + output_file_ch1;
                    wait_command_finish = system( split_command.c_str() );
                }

                delete header;
                file.close();
            }
            else
            {
                std::cout << "open failed. " << std::endl;
            }
            
            return 0;
        }

    private:
        Codecs *codecs_;
        wav_hdr wav_hdr_;
    };

    Codecs::Codecs()
        : private_( new Private( this ) )
    {
    }

    Codecs::~Codecs()
    {
        if ( nullptr != private_ ) {
            delete private_;
        }
          
    }

    int Codecs::wavSplit( const std::string path, const std::string filename )
    {
        return private_->wavSplit( path, filename );
    }
};