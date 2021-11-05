#include "codecs.h"

#include "memory"
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include <log4cxx/logger.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
}

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

        typedef struct FilteringContext
        {
            AVFilterContext *buffersink_ctx;
            AVFilterContext *buffersrc_ctx;
            AVFilterGraph *filter_graph;

            AVPacket *enc_pkt;
            AVFrame *filtered_frame;
        } FilteringContext;

        typedef struct StreamContext
        {
            AVCodecContext *dec_ctx;
            AVCodecContext *enc_ctx;

            AVFrame *dec_frame;
        } StreamContext;

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
        // inline bool findG729Codec()
        // {
        //     const AVCodec *cd = avcodec_find_decoder(AV_CODEC_ID_G729);
        //     if (!CLONE_DETACHED)
        //     {
        //         std::cout << "error" << std::endl;
        //         return false;
        //     }
        //     return true;
        // }

        // inline int open_input_file(const std::string filename)
        // {
        //     int ret;
        //     unsigned int i;

        //     ifmt_ctx_ = NULL;
        //     if ((ret = avformat_open_input(&ifmt_ctx_, filename.c_str(), NULL, NULL)) < 0)
        //     {
        //         av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        //         return ret;
        //     }

        //     if ((ret = avformat_find_stream_info(ifmt_ctx_, NULL)) < 0)
        //     {
        //         av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        //         return ret;
        //     }

        //     stream_ctx_ = (StreamContext*)av_calloc(ifmt_ctx_->nb_streams, sizeof(*stream_ctx_));
        //     if (!stream_ctx_)
        //         return AVERROR(ENOMEM);

        //     for (i = 0; i < ifmt_ctx_->nb_streams; i++)
        //     {
        //         AVStream *stream = ifmt_ctx_->streams[i];
        //         const AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
        //         AVCodecContext *codec_ctx;
        //         if (!dec)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
        //             return AVERROR_DECODER_NOT_FOUND;
        //         }
        //         codec_ctx = avcodec_alloc_context3(dec);
        //         if (!codec_ctx)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
        //             return AVERROR(ENOMEM);
        //         }
        //         ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
        //                                        "for stream #%u\n",
        //                    i);
        //             return ret;
        //         }
        //         /* Reencode video & audio and remux subtitles etc. */
        //         if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
        //         {
        //             if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
        //                 codec_ctx->framerate = av_guess_frame_rate(ifmt_ctx_, stream, NULL);
        //             /* Open decoder */
        //             ret = avcodec_open2(codec_ctx, dec, NULL);
        //             if (ret < 0)
        //             {
        //                 av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
        //                 return ret;
        //             }
        //         }
        //         stream_ctx_[i].dec_ctx = codec_ctx;

        //         stream_ctx_[i].dec_frame = av_frame_alloc();
        //         if (!stream_ctx_[i].dec_frame)
        //             return AVERROR(ENOMEM);
        //     }

        //     av_dump_format(ifmt_ctx_, 0, filename.c_str(), 0);
        //     return 0;
        // }

        // inline int open_output_file(const std::string filename)
        // {
        //     AVStream *out_stream;
        //     AVStream *in_stream;
        //     AVCodecContext *dec_ctx, *enc_ctx;
        //     const AVCodec *encoder;
        //     int ret;
        //     unsigned int i;

        //     ofmt_ctx_ = NULL;
        //     avformat_alloc_output_context2(&ofmt_ctx_, NULL, NULL, filename.c_str());
        //     if (!ofmt_ctx_) {
        //         av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        //         return AVERROR_UNKNOWN;
        //     }


        //     for (i = 0; i < ifmt_ctx_->nb_streams; i++) {
        //         out_stream = avformat_new_stream(ofmt_ctx_, NULL);
        //         if (!out_stream) {
        //             av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
        //             return AVERROR_UNKNOWN;
        //         }

        //         in_stream = ifmt_ctx_->streams[i];
        //         dec_ctx = stream_ctx_[i].dec_ctx;

        //         if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
        //                 || dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        //             /* in this example, we choose transcoding to same codec */
        //             encoder = avcodec_find_encoder(dec_ctx->codec_id);
        //             if (!encoder) {
        //                 av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
        //                 return AVERROR_INVALIDDATA;
        //             }
        //             enc_ctx = avcodec_alloc_context3(encoder);
        //             if (!enc_ctx) {
        //                 av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
        //                 return AVERROR(ENOMEM);
        //             }

        //             /* In this example, we transcode to same properties (picture size,
        //             * sample rate etc.). These properties can be changed for output
        //             * streams easily using filters */
        //             if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        //                 enc_ctx->height = dec_ctx->height;
        //                 enc_ctx->width = dec_ctx->width;
        //                 enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
        //                 /* take first format from list of supported formats */
        //                 if (encoder->pix_fmts)
        //                     enc_ctx->pix_fmt = encoder->pix_fmts[0];
        //                 else
        //                     enc_ctx->pix_fmt = dec_ctx->pix_fmt;
        //                 /* video time_base can be set to whatever is handy and supported by encoder */
        //                 enc_ctx->time_base = av_inv_q(dec_ctx->framerate);
        //             } else {
        //                 enc_ctx->sample_rate = dec_ctx->sample_rate;
        //                 enc_ctx->channel_layout = dec_ctx->channel_layout;
        //                 enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
        //                 /* take first format from list of supported formats */
        //                 enc_ctx->sample_fmt = encoder->sample_fmts[0];
        //                 enc_ctx->time_base = (AVRational){1, enc_ctx->sample_rate};
        //             }

        //             if (ofmt_ctx_->oformat->flags & AVFMT_GLOBALHEADER)
        //                 enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        //             /* Third parameter can be used to pass settings to encoder */
        //             ret = avcodec_open2(enc_ctx, encoder, NULL);
        //             if (ret < 0) {
        //                 av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
        //                 return ret;
        //             }
        //             ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
        //             if (ret < 0) {
        //                 av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
        //                 return ret;
        //             }

        //             out_stream->time_base = enc_ctx->time_base;
        //             stream_ctx_[i].enc_ctx = enc_ctx;
        //         } else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
        //             av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
        //             return AVERROR_INVALIDDATA;
        //         } else {
        //             /* if this stream must be remuxed */
        //             ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        //             if (ret < 0) {
        //                 av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
        //                 return ret;
        //             }
        //             out_stream->time_base = in_stream->time_base;
        //         }

        //     }
        //     av_dump_format(ofmt_ctx_, 0, filename.c_str(), 1);

        //     if (!(ofmt_ctx_->oformat->flags & AVFMT_NOFILE)) {
        //         ret = avio_open(&ofmt_ctx_->pb, filename.c_str(), AVIO_FLAG_WRITE);
        //         if (ret < 0) {
        //             LOG4CXX_DEBUG( Logger_, "Could not open output file" << filename );
        //             return ret;
        //         }
        //     }

        //     /* init muxer, write output file header */
        //     ret = avformat_write_header(ofmt_ctx_, NULL);
        //     if (ret < 0) {
        //         av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        //         return ret;
        //     }

        //     return 0;
        // }
        
        // inline int init_filter(FilteringContext *fctx, AVCodecContext *dec_ctx,
        //                        AVCodecContext *enc_ctx, const char *filter_spec)
        // {
        //     char args[512];
        //     int ret = 0;
        //     const AVFilter *buffersrc = NULL;
        //     const AVFilter *buffersink = NULL;
        //     AVFilterContext *buffersrc_ctx = NULL;
        //     AVFilterContext *buffersink_ctx = NULL;
        //     AVFilterInOut *outputs = avfilter_inout_alloc();
        //     AVFilterInOut *inputs = avfilter_inout_alloc();
        //     AVFilterGraph *filter_graph = avfilter_graph_alloc();

        //     if (!outputs || !inputs || !filter_graph)
        //     {
        //         ret = AVERROR(ENOMEM);
        //         goto end;
        //     }

        //     if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
        //     {
        //         buffersrc = avfilter_get_by_name("buffer");
        //         buffersink = avfilter_get_by_name("buffersink");
        //         if (!buffersrc || !buffersink)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
        //             ret = AVERROR_UNKNOWN;
        //             goto end;
        //         }

        //         snprintf(args, sizeof(args),
        //                  "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        //                  dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
        //                  dec_ctx->time_base.num, dec_ctx->time_base.den,
        //                  dec_ctx->sample_aspect_ratio.num,
        //                  dec_ctx->sample_aspect_ratio.den);

        //         ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
        //                                            args, NULL, filter_graph);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        //             goto end;
        //         }

        //         ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
        //                                            NULL, NULL, filter_graph);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        //             goto end;
        //         }

        //         ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
        //                              (uint8_t *)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
        //                              AV_OPT_SEARCH_CHILDREN);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        //             goto end;
        //         }
        //     }
        //     else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
        //     {
        //         buffersrc = avfilter_get_by_name("abuffer");
        //         buffersink = avfilter_get_by_name("abuffersink");
        //         if (!buffersrc || !buffersink)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
        //             ret = AVERROR_UNKNOWN;
        //             goto end;
        //         }

        //         if (!dec_ctx->channel_layout)
        //             dec_ctx->channel_layout =
        //                 av_get_default_channel_layout(dec_ctx->channels);
        //         snprintf(args, sizeof(args),
        //                  "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
        //                  dec_ctx->time_base.num, dec_ctx->time_base.den, dec_ctx->sample_rate,
        //                  av_get_sample_fmt_name(dec_ctx->sample_fmt),
        //                  dec_ctx->channel_layout);
        //         ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
        //                                            args, NULL, filter_graph);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
        //             goto end;
        //         }

        //         ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
        //                                            NULL, NULL, filter_graph);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
        //             goto end;
        //         }

        //         ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
        //                              (uint8_t *)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
        //                              AV_OPT_SEARCH_CHILDREN);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
        //             goto end;
        //         }

        //         ret = av_opt_set_bin(buffersink_ctx, "channel_layouts",
        //                              (uint8_t *)&enc_ctx->channel_layout,
        //                              sizeof(enc_ctx->channel_layout), AV_OPT_SEARCH_CHILDREN);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
        //             goto end;
        //         }

        //         ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
        //                              (uint8_t *)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
        //                              AV_OPT_SEARCH_CHILDREN);
        //         if (ret < 0)
        //         {
        //             av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
        //             goto end;
        //         }
        //     }
        //     else
        //     {
        //         ret = AVERROR_UNKNOWN;
        //         goto end;
        //     }

        //     /* Endpoints for the filter graph. */
        //     outputs->name = av_strdup("in");
        //     outputs->filter_ctx = buffersrc_ctx;
        //     outputs->pad_idx = 0;
        //     outputs->next = NULL;

        //     inputs->name = av_strdup("out");
        //     inputs->filter_ctx = buffersink_ctx;
        //     inputs->pad_idx = 0;
        //     inputs->next = NULL;

        //     if (!outputs->name || !inputs->name)
        //     {
        //         ret = AVERROR(ENOMEM);
        //         goto end;
        //     }

        //     if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
        //                                         &inputs, &outputs, NULL)) < 0)
        //         goto end;

        //     if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        //         goto end;

        //     /* Fill FilteringContext */
        //     fctx->buffersrc_ctx = buffersrc_ctx;
        //     fctx->buffersink_ctx = buffersink_ctx;
        //     fctx->filter_graph = filter_graph;

        // end:
        //     avfilter_inout_free(&inputs);
        //     avfilter_inout_free(&outputs);

        //     return ret;
        // }

        // inline int init_filters(void)
        // {
        //     const char *filter_spec;
        //     unsigned int i;
        //     int ret;
        //     filter_ctx_ = ( FilteringContext* )av_malloc_array( ifmt_ctx_->nb_streams, sizeof( *filter_ctx_ ) );
        //     if (!filter_ctx_)
        //         return AVERROR(ENOMEM);

        //     for (i = 0; i < ifmt_ctx_->nb_streams; i++)
        //     {
        //         filter_ctx_[i].buffersrc_ctx = NULL;
        //         filter_ctx_[i].buffersink_ctx = NULL;
        //         filter_ctx_[i].filter_graph = NULL;
        //         if (!(ifmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO || ifmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO))
        //             continue;

        //         if (ifmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        //             filter_spec = "null"; /* passthrough (dummy) filter for video */
        //         else
        //             filter_spec = "anull"; /* passthrough (dummy) filter for audio */
        //         ret = init_filter(&filter_ctx_[i], stream_ctx_[i].dec_ctx,
        //                           stream_ctx_[i].enc_ctx, filter_spec);
        //         if (ret)
        //             return ret;

        //         filter_ctx_[i].enc_pkt = av_packet_alloc();
        //         if (!filter_ctx_[i].enc_pkt)
        //             return AVERROR(ENOMEM);

        //         filter_ctx_[i].filtered_frame = av_frame_alloc();
        //         if (!filter_ctx_[i].filtered_frame)
        //             return AVERROR(ENOMEM);
        //     }
        //     return 0;
        // }

        // inline int encode_write_frame(unsigned int stream_index, int flush)
        // {
        //     StreamContext *stream = &stream_ctx_[stream_index];
        //     FilteringContext *filter = &filter_ctx_[stream_index];
        //     AVFrame *filt_frame = flush ? NULL : filter->filtered_frame;
        //     AVPacket *enc_pkt = filter->enc_pkt;
        //     int ret;

        //     av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
        //     /* encode filtered frame */
        //     av_packet_unref(enc_pkt);

        //     ret = avcodec_send_frame(stream->enc_ctx, filt_frame);

        //     if (ret < 0)
        //         return ret;

        //     while (ret >= 0)
        //     {
        //         ret = avcodec_receive_packet(stream->enc_ctx, enc_pkt);

        //         if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        //             return 0;

        //         /* prepare packet for muxing */
        //         enc_pkt->stream_index = stream_index;
        //         av_packet_rescale_ts(enc_pkt,
        //                              stream->enc_ctx->time_base,
        //                              ofmt_ctx_->streams[stream_index]->time_base);

        //         av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");
        //         /* mux encoded frame */
        //         ret = av_interleaved_write_frame(ofmt_ctx_, enc_pkt);
        //     }

        //     return ret;
        // }

        // inline int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index)
        // {
        //     FilteringContext *filter = &filter_ctx_[stream_index];
        //     int ret;

        //     av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
        //     /* push the decoded frame into the filtergraph */
        //     ret = av_buffersrc_add_frame_flags(filter->buffersrc_ctx,
        //                                        frame, 0);
        //     if (ret < 0)
        //     {
        //         av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        //         return ret;
        //     }

        //     /* pull filtered frames from the filtergraph */
        //     while (1)
        //     {
        //         av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        //         ret = av_buffersink_get_frame(filter->buffersink_ctx,
        //                                       filter->filtered_frame);
        //         if (ret < 0)
        //         {
        //             /* if no more frames for output - returns AVERROR(EAGAIN)
        //             * if flushed and no more frames for output - returns AVERROR_EOF
        //             * rewrite retcode to 0 to show it as normal procedure completion
        //             */
        //             if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        //                 ret = 0;
        //             break;
        //         }

        //         filter->filtered_frame->pict_type = AV_PICTURE_TYPE_NONE;
        //         ret = encode_write_frame(stream_index, 0);
        //         av_frame_unref(filter->filtered_frame);
        //         if (ret < 0)
        //             break;
        //     }

        //     return ret;
        // }

        // inline int flush_encoder(unsigned int stream_index)
        // {
        //     if (!(stream_ctx_[stream_index].enc_ctx->codec->capabilities &
        //           AV_CODEC_CAP_DELAY))
        //         return 0;

        //     av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
        //     return encode_write_frame(stream_index, 1);
        // }

        // inline void decodeg729(const std::string input_path, const std::string output_path)
        // {
        //     int ret;
        //     AVPacket *packet = NULL;

        //     unsigned int stream_index;
        //     unsigned int index;

        //     if ((ret = open_input_file(input_path) < 0))
        //         return;
        //     if ((ret = open_output_file(output_path) < 0))
        //         return;    
        //     if ((ret = init_filters()) < 0)
        //         return;
        //     if (!(packet = av_packet_alloc()))
        //         return;

        //     while( 1 ) {
        //         if( ( ret = av_read_frame( ifmt_ctx_, packet) ) < 0 )
        //             break;
        //         stream_index = packet->stream_index;
        //         LOG4CXX_DEBUG( Logger_, "Demuxer gave frame of stream_index" << stream_index );

        //         if ( filter_ctx_[stream_index].filter_graph ) {
        //             StreamContext *stream = &stream_ctx_[stream_index];

        //             LOG4CXX_DEBUG( Logger_, "Going to reencode&filter the frame" );

        //             av_packet_rescale_ts( packet,
        //                                     ifmt_ctx_->streams[stream_index]->time_base,
        //                                     stream->dec_ctx->time_base );
        //             ret = avcodec_send_packet( stream->dec_ctx, packet );
        //             if ( ret < 0 ) {
        //                 LOG4CXX_DEBUG( Logger_, "Going to reencode&filter the frame" );
        //                 break;
        //             }
                    
        //             while ( ret >= 0 ) {
        //                 ret = avcodec_receive_frame(stream->dec_ctx, stream->dec_frame );
        //                 if ( ret == AVERROR_EOF || ret == AVERROR( EAGAIN ) )
        //                     break;
        //                 else if ( ret < 0 )
        //                     return;

        //                 stream->dec_frame->pts = stream->dec_frame->best_effort_timestamp;
        //                 ret = filter_encode_write_frame( stream->dec_frame, stream_index )    ;
        //                 if ( ret < 0 )
        //                     return;
        //             }
        //         } else {
        //             av_packet_rescale_ts( packet,
        //                                     ifmt_ctx_->streams[stream_index]->time_base,
        //                                     ofmt_ctx_->streams[stream_index]->time_base );
        //             ret = av_interleaved_write_frame( ofmt_ctx_, packet );
        //             if ( ret < 0 )
        //                 return;
        //         }
        //         av_packet_unref( packet );
        //     }

        //     for ( index = 0; index < ifmt_ctx_->nb_streams; index++ ) {
        //         if( !filter_ctx_[index].filter_graph ) 
        //             continue;
        //         ret = filter_encode_write_frame( NULL, index );
        //         if ( ret < 0 ) {
        //             LOG4CXX_DEBUG( Logger_, "Flushing filter failed." );
        //             return;
        //         }

        //         ret = flush_encoder( index );
        //         if ( ret < 0 ) {
        //             LOG4CXX_DEBUG( Logger_, "Flushing encoder failed." );
        //             return;
        //         }
        //     }   
        //     av_write_trailer( ofmt_ctx_ );
        // }

        // inline u_int64_t getFileSize(FILE *fp)
        // {
        //     u_int64_t position, size;
        //     position = ftell(fp);
        //     fseek(fp, 0, position);
        //     size = ftell(fp);
        //     fseek(fp, position, SEEK_SET);
        //     return size;
        // }

        inline int wavSplit(const std::string input_path, const std::string output_path)
        {
            int total_len = 0;
            int header_size = sizeof(wav_hdr_);

            uint32_t bytes_size;
            std::ifstream file(input_path.c_str(), std::ifstream::in | std::ifstream::binary);

            if (file.is_open())
            {
                file.seekg(0, file.end);
                total_len = (int)file.tellg();
                file.seekg(0, file.beg);

                unsigned char *header = new unsigned char[header_size];

                file.read((char *)&wav_hdr_, header_size);

                if (0 == strncmp("RIFF", (char *)wav_hdr_.RIFF, 4))
                {
                    LOG4CXX_DEBUG(Logger_, "Is Wav File.");

                    if (0 == strncmp("SIGN", (char *)wav_hdr_.fmt, 4))
                    {
                        LOG4CXX_DEBUG(Logger_, "Is G729 Format.");
                        file.seekg(-(header_size - 16), file.cur);
                        std::cout << file.tellg() << std::endl;

                        decodeg729( input_path, output_path );
                    }
                    else if (0 == strncmp("fmt ", (char *)wav_hdr_.fmt, 4))
                    {
                        LOG4CXX_DEBUG(Logger_, "Is Wav Format.");
                        std::cout << file.tellg() << std::endl;
                    }
                    else
                    {
                        LOG4CXX_DEBUG(Logger_, "Is Invalid Format.");
                    }
                }
                else
                {
                    LOG4CXX_DEBUG(Logger_, "Is PCM File.");
                    unsigned char *buffer = (unsigned char *)malloc(total_len);
                    file.seekg(0, file.beg);
                    std::cout << file.tellg() << std::endl;
                }

                delete header;
                delete buffer;
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
        AVCodec *avcodec_;
        AVFormatContext *ifmt_ctx_;
        AVFormatContext *ofmt_ctx_;
        FilteringContext *filter_ctx_;
        StreamContext *stream_ctx_;
    };

    Codecs::Codecs()
        : private_(new Private(this))
    {
    }

    Codecs::~Codecs()
    {
        delete private_;
    }

    int Codecs::wavSplit(const std::string file_path, const std::string output_path)
    {
        return private_->wavSplit(file_path, output_path);
    }

};