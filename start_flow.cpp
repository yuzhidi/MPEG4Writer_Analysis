���ʼǾ���һĿ��Ȼ��
û�ø��ֻ�ͼ���ߣ�����Ϊ���������߼���������һ�塣
(���ɣ����ڻ᲻���޸ĵĲ��֣�ÿ�����ֻ��һ�����ݣ����ڵ���������֮���ǣ�����Ĵ������Ϊ����Ĵ���չ�������úܿ����޷��۵�:�취����{})                                                                                            
��if�����Ĵ��룬�����ж��Ƿ�ִ�У�Ҫִ���ٿ���

��ѭ��Ҫ����ĵط���
startWriterThread�еĳ�ʼ����mChunkInfos�ǲ���Ӧ��Ҫ����
vEncSetForceIframe(true)

track,��trackinfo��ʹ��(�ܽ��������̴�������)��
List<Track *> mTracks;//ͷ�ļ�
MPEG4Writer::addSource()��������Track���󣬲��ѵ�ַ����mTracks
MPEG4Writer::startWriterThread()����ChunkInfo����mTracksȡ��Track����,��ΪChunkInfo��owner,����ChunkInfo����mChunkInfos
MPEG4Writer::bufferChunk(const Chunk& chunk)��

�����ⲽ�ܹؼ����Ѱ�����ʱ��sample���ݵ�chunk���ŵ���Ӧ��
        if (chunk.mTrack == it->mTrack) {  // Found owner
            it->mChunks.push_back(chunk);

��δ�mChunks��ȡ��chunkд��cache���뿴��һƪ�ĵ���findChunkToWrite(&chunk) ��ע�͡�
##########################################################################################################################################################

�ٿ��������̣�
+---------------------------+
StagefrightRecorder::start()
+---------------------------+

��������ܼ򵥣�


�����ͣ����ָ�
    if (mPaused) {
        return resume();
    } 
												
startMPEG4Recording();

	+-------------------------------------------------+					
     status_t StagefrightRecorder::startMPEG4Recording()     
	+-------------------------------------------------+											
	//��Ҫ����3������
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
																													       ------------------>   status_t StagefrightRecorder::setupMediaSource( //�����cameraȡ����
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
																																				//�����ɶ�Ӧ��track,�ⲿ�ĵ������̣��˴�����video track
																																				    Track *track = new Track(this, source, 1 + mTracks.size());
																																					mTracks.push_back(track);  
																																					
																																					mVideoQualityController = new VideoQualityController(this, source);
																																				}
																				
																				*totalBitRate += videoBitRate;
																				
																				setupAudioEncoder(writer);             ------------------------>
																																					{+----------------------------------------------------------------+
																																					status_t StagefrightRecorder::setupAudioEncoder(const sp<MediaWriter>& writer)
																																					+----------------------------------------------------------------+
																																					 writer->addSource(audioEncoder); //������audio track
																																					}
																				*totalBitRate += mAudioBitRate;
																				//writer��ʼ��mInterleaveDurationUs, mLatitudex10000, mLongitudex10000, mMaxFileDurationUs
																				  mMaxFileSizeBytes, mStartTimeOffsetMs, 
																				  // Ҫע��mStartTimeOffsetMs���ļ��л�ʱ�Ƿ���Ҫ����
																				mStartTimeOffsetMs = mEncoderProfiles->getStartTimeOffsetMs(mCameraId);
																				}
	sp<MetaData> meta = new MetaData;
	 //Ҫ������ȡ��totalBitRate����������á�
	setupMPEG4MetaData(startTimeUs, totalBitRate, &meta);
																			{+-------------------------------------------------+
															----------->     void StagefrightRecorder::setupMPEG4MetaData
																			+-------------------------------------------------+
																			Ҫ����MetaData�е�kKeyXXX�����ʹ�ã������
																			}
	//mWriter���ڵ�һ�����������ɵģ�meta���ڵڶ�������������
	err = mWriter->start(meta.get());

																			{+-------------------------------------------------+
															----------->     status_t MPEG4Writer::start(MetaData *param)
																			+-------------------------------------------------+
																				MetaData *param��ʼ�� mMaxFileSizeLimitBytes,
																				mIsRealTimeRecording, mVideoQualityController
																				mArtistTag, mAlbumTag
																			/*
																				* mWriteMoovBoxToMemory is true if the amount of data in moov box is
																				* smaller than the reserved free space at the beginning of a file, AND
																				* when the content of moov box is constructed. 
																				Note that video/audio
																				* frame data is always written to the file but not in the memory.    leo note: video/audio frame data��ֱ��д���ļ�
																				*
																				* Before stop()/reset() is called, mWriteMoovBoxToMemory is always
																				* false. When reset() is called at the end of a recording session,
																				* Moov box needs to be constructed.									leo note:¼�ƽ�����ʱ��Ź���Moov box
																				*
																				* 1) Right before a moov box is constructed, mWriteMoovBoxToMemory   leo note:mWriteMoovBoxToMemoryӰ��mStreamableFile
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
																				leo note:���еڶ���������������MOOV box��reserved free space��reserved free spaceӦ���ǹ��õģ��Ͳ���ѡ�ˡ�
																			     writeFtypBox(param);
																				 ���˸о�mEstimatedMoovBoxSize =0 ��ʾmoovbox�����ļ�β
																				 //log:mTryStreamableFile:0, mStreamableFile:28, start, 1148
																				mMdatOffset = mOffset;
																				write Mdat
																				
																				startWriterThread();

																																		{+-------------------------------------------------+
															                                                             ----------->     status_t MPEG4Writer::startWriterThread()
															                                                             				+-------------------------------------------------+
																																		// 1. ����mTracks�е�Track��mChunkInfos �����Ӧ��mChunkInfos
																																			mDone = false;
																																			mIsFirstChunk = true;
																																			mDriftTimeUs = 0;
																																			for (List<Track *>::iterator it = mTracks.begin();
																																				it != mTracks.end(); ++it) {
																																				ChunkInfo info;
																																				info.mTrack = *it;             // leo ��һ������Ҫ������
																																				info.mPrevChunkTimestampUs = 0;// Previous chunk timestamp that has been written
																																				info.mMaxInterChunkDurUs = 0;// // Max time interval between neighboring chunks
																																				mChunkInfos.push_back(info);   // ͷ�ļ�����List<ChunkInfo> mChunkInfos;
																																			}
																																			mWriterThreadExit = false;
																																		// 2.����д���߳�
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
															                                                                                                                           				MetaData *params�е�����������ʼ���������
															                                                                                                                           				kKeyTime��kKeyRotation�� kKeySlowMotionSpeedValue�� kKeyFrameRate
																																																	
															                                                                                                                           				meta->setInt64(kKeyTime, startTimeUs);��Ϊ��һ��
															                                                                                                                           				// To be called before any other methods on this object, except getFormat().
															                                                                                                                                          //������ܻ�Ҫ�һ�£��ؼ���ʱ��͵�һ֡
																								                                                       							                  	mSource->start(meta.get());(����)�Ĳ���
																																																	pthread_create(&mThread, &attr, ThreadWrapper, this);
																																																													-------->  track->threadEntry();
												                               																									             	}
																			}

																													
																													
																													
// battery dataɶ��˼��
uint32_t params = IMediaPlayerService::kBatteryDataCodecStarted;
addBatteryData(params);
}