做笔记就是一目了然。
没用各种画图工具，是因为这样，集逻辑，代码于一体。
(技巧，对于会不断修改的部分，每行最好只有一个内容，便于调整，不好之处是，顶层的代码会因为后面的代码展开而拉得很开，无法折叠:办法，用{})                                                                                            
有if包裹的代码，首先判断是否执行，要执行再看。

做循环要留意的地方：
startWriterThread中的初始化，mChunkInfos是不是应该要两份
vEncSetForceIframe(true)

track,与trackinfo的使用(能将整个流程串起来。)：
List<Track *> mTracks;//头文件
MPEG4Writer::addSource()创建两个Track对象，并把地址放入mTracks
MPEG4Writer::startWriterThread()创建ChunkInfo。从mTracks取出Track对象,作为ChunkInfo的owner,并将ChunkInfo放入mChunkInfos
MPEG4Writer::bufferChunk(const Chunk& chunk)中

下面这步很关键：把包含了时间sample数据的chunk，放到对应的
        if (chunk.mTrack == it->mTrack) {  // Found owner
            it->mChunks.push_back(chunk);

如何从mChunks中取出chunk写入cache，请看另一篇文档中findChunkToWrite(&chunk) 的注释。
##########################################################################################################################################################

再看整个流程：
+---------------------------+
StagefrightRecorder::start()
+---------------------------+

这个函数很简单：


如果暂停，则恢复
    if (mPaused) {
        return resume();
    } 
												
startMPEG4Recording();

	+-------------------------------------------------+					
     status_t StagefrightRecorder::startMPEG4Recording()     
	+-------------------------------------------------+											
	//主要调用3个函数
{	status_t err = setupMPEG4Recording(						
					mOutputFd, mVideoWidth, mVideoHeight,		
					mVideoBitRate, &totalBitRate, &mWriter);				
																		   {+-------------------------------------------------+
															----------->     status_t StagefrightRecorder::setupMPEG4Recording(
																			+-------------------------------------------------+
																			   sp<MediaWriter> writer = new MPEG4Writer(outputFd);
										                                                                            							{+-------------------------------------------------+
										                                                                            			----------->     MPEG4Writer::MPEG4Writer(const char *filename)
										                                                                            							+-------------------------------------------------+																		
																																				mFd = open(filename, O_CREAT | O_LARGEFILE | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
																																				mWriterCacheSize = DEFAULT_FILE_CACHE_SIZE;
																																				mCacheWriter = new MPEG4FileCacheWriter(mFd, mWriterCacheSize);
																																				mCacheWriter->setOwner(this);
																																				mBufferedDataSize = 0;
																																				mVideoQualityController = NULL;
																																				}

																				sp<MediaSource> mediaSource;
																				err = setupMediaSource(&mediaSource);
																																				{+-------------------------------------------------+
																													       ------------------>   status_t StagefrightRecorder::setupMediaSource( //解决从camera取数据
																																				 +-------------------------------------------------+
																																					sp<CameraSource> cameraSource;
																																					status_t err = setupCameraSource(&cameraSource);
																																				}
																				sp<MediaSource> encoder;
																				err = setupVideoEncoder(mediaSource, videoBitRate, &encoder);  //encoder soruce is camera
																				
																				writer->addSource(encoder);	// writer source is encoder
										                                                                            							{+----------------------------------------------------------------+
										                                                                            			----------->     status_t MPEG4Writer::addSource(const sp<MediaSource> &source)
										                                                                            							+----------------------------------------------------------------+
																																				//会生成对应的track,外部的调用流程，此处生成video track
																																				    Track *track = new Track(this, source, 1 + mTracks.size());
																																					mTracks.push_back(track);  
																																					
																																					mVideoQualityController = new VideoQualityController(this, source);
																																				}
																				
																				*totalBitRate += videoBitRate;
																				
																				setupAudioEncoder(writer);             ------------------------>
																																					{+----------------------------------------------------------------+
																																					status_t StagefrightRecorder::setupAudioEncoder(const sp<MediaWriter>& writer)
																																					+----------------------------------------------------------------+
																																					 writer->addSource(audioEncoder); //会生成audio track
																																					}
																				*totalBitRate += mAudioBitRate;
																				//writer初始化mInterleaveDurationUs, mLatitudex10000, mLongitudex10000, mMaxFileDurationUs
																				  mMaxFileSizeBytes, mStartTimeOffsetMs, 
																				  // 要注意mStartTimeOffsetMs在文件切换时是否需要更新
																				mStartTimeOffsetMs = mEncoderProfiles->getStartTimeOffsetMs(mCameraId);
																				}
	sp<MetaData> meta = new MetaData;
	 //要先上面取得totalBitRate，下面才能用。
	setupMPEG4MetaData(startTimeUs, totalBitRate, &meta);
																			{+-------------------------------------------------+
															----------->     void StagefrightRecorder::setupMPEG4MetaData
																			+-------------------------------------------------+
																			要明白MetaData中的kKeyXXX是如何使用，保存的
																			}
	//mWriter是在第一个函数中生成的，meta是在第二个函数中生成
	err = mWriter->start(meta.get());

																			{+-------------------------------------------------+
															----------->     status_t MPEG4Writer::start(MetaData *param)
																			+-------------------------------------------------+
																				MetaData *param初始化 mMaxFileSizeLimitBytes,
																				mIsRealTimeRecording, mVideoQualityController
																				mArtistTag, mAlbumTag
																			/*
																				* mWriteMoovBoxToMemory is true if the amount of data in moov box is
																				* smaller than the reserved free space at the beginning of a file, AND
																				* when the content of moov box is constructed. 
																				Note that video/audio
																				* frame data is always written to the file but not in the memory.    leo note: video/audio frame data是直接写到文件
																				*
																				* Before stop()/reset() is called, mWriteMoovBoxToMemory is always
																				* false. When reset() is called at the end of a recording session,
																				* Moov box needs to be constructed.									leo note:录制结束的时候才构建Moov box
																				*
																				* 1) Right before a moov box is constructed, mWriteMoovBoxToMemory   leo note:mWriteMoovBoxToMemory影响mStreamableFile
																				* to set to mStreamableFile so that if
																				* the file is intended to be streamable, it is set to true;
																				* otherwise, it is set to false. When the value is set to false,   leo note:mStreamableFile is false, to file
																				* all the content of the moov box is written immediately to
																				* the end of the file. When the value is set to true, all the
																				* content of the moov box is written to an in-memory cache,          
																				* mMoovBoxBuffer, util the following condition happens. Note		leo note:mStreamableFile is true, mMoovBoxBuffer
																				* that the size of the in-memory cache is the same as the
																				* reserved free space at the beginning of the file.
																				*/	
																				leo note:还有第二，第三条，关于MOOV box与reserved free space。reserved free space应该是够用的，就不节选了。
																			     writeFtypBox(param);
																				 个人感觉mEstimatedMoovBoxSize =0 表示moovbox加在文件尾
																				 //log:mTryStreamableFile:0, mStreamableFile:28, start, 1148
																				mMdatOffset = mOffset;
																				write Mdat
																				
																				startWriterThread();

																																		{+-------------------------------------------------+
															                                                             ----------->     status_t MPEG4Writer::startWriterThread()
															                                                             				+-------------------------------------------------+
																																		// 1. 根据mTracks中的Track，mChunkInfos 放入对应的mChunkInfos
																																			mDone = false;
																																			mIsFirstChunk = true;
																																			mDriftTimeUs = 0;
																																			for (List<Track *>::iterator it = mTracks.begin();
																																				it != mTracks.end(); ++it) {
																																				ChunkInfo info;
																																				info.mTrack = *it;             // leo 这一步很重要！！！
																																				info.mPrevChunkTimestampUs = 0;// Previous chunk timestamp that has been written
																																				info.mMaxInterChunkDurUs = 0;// // Max time interval between neighboring chunks
																																				mChunkInfos.push_back(info);   // 头文件定义List<ChunkInfo> mChunkInfos;
																																			}
																																			mWriterThreadExit = false;
																																		// 2.开启写入线程
																																			pthread_create(&mThread, &attr, ThreadWrapper, this);
																																																-------->    writer->threadFunc();
																																			pthread_attr_destroy(&attr);
																																			mWriterThreadStarted = true;
																																			}

																				startTracks(param);
																																							
																																		{+-------------------------------------------------+
															                                                             ----------->     status_t MPEG4Writer::startTracks(MetaData *params)
															                                                                    		+-------------------------------------------------+
																																		    for (List<Track *>::iterator it = mTracks.begin();
																																			it != mTracks.end(); ++it) {
																																				ALOGD("call mTracks->start() [%s], [%d]", __FUNCTION__, __LINE__);
																																			status_t err = (*it)->start(params);
																																		}

																								                                                       												{+-------------------------------------------------+
															                                                                                                                           ----------->     status_t MPEG4Writer::Track::start(MetaData *params)
															                                                                                                                           				+-------------------------------------------------+
															                                                                                                                           				vEncSetForceIframe(true)
															                                                                                                                           				MetaData *params中的数据用来初始化类的属性
															                                                                                                                           				kKeyTime，kKeyRotation， kKeySlowMotionSpeedValue， kKeyFrameRate
																																																	
															                                                                                                                           				meta->setInt64(kKeyTime, startTimeUs);作为下一步
															                                                                                                                           				// To be called before any other methods on this object, except getFormat().
															                                                                                                                                          //这里可能还要深究一下，关键是时间和第一帧
																								                                                       							                  	mSource->start(meta.get());(接上)的参数
																																																	pthread_create(&mThread, &attr, ThreadWrapper, this);
																																																													-------->  track->threadEntry();
												                               																									             	}
																			}

																													
																													
																													
// battery data啥意思？
uint32_t params = IMediaPlayerService::kBatteryDataCodecStarted;
addBatteryData(params);
}