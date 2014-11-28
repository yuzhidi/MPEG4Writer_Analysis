
	+-------------------------------------------------+					
     status_t StagefrightRecorder::stop()     
	+-------------------------------------------------+	
	
                                                      							{+-------------------------------------------------+
                                                       			----------->     MPEG4Writer::stop()
                                                       							+-------------------------------------------------+	
																					reset()
																				    for (List<Track *>::iterator it = mTracks.begin();
																					it != mTracks.end(); ++it) {
																					status_t status = (*it)->stop();
																					maxDurationUs, minDurationUs
																					stopWriterThread();
																					release();
																					writeMetaData();
																					
																					
																					
																					文件大小达到上限的停止 exceedsFileSizeLimit()
																					
																					//resume，pause目前都遇不到
																					signalResumed(hasMultipleTracks);
																					mPauseCondition.signal();
																					
																					
																					
																					

																					
//判断文件大小的逻辑:
		//每读取一个sample,就要增加mMdatSizeBytes,
        mMdatSizeBytes += sampleSize;
		//同时增加moov结构的大小
        updateTrackSizeEstimate();												estimate：估计，预测
                                                      							{+-------------------------------------------------+
                                                       			----------->    void MPEG4Writer::Track::updateTrackSizeEstimate()
                                                       							+-------------------------------------------------+	
																				mEstimatedTrackSizeBytes = mMdatSizeBytes;
																				
																				        // Reserved free space is not large enough to hold
																						// all meta data and thus wasted.
																						mEstimatedTrackSizeBytes += mStscTableEntries->count() * 12 +  // stsc box size
																													mStssTableEntries->count() * 4 +   // stss box size
																													mSttsTableEntries->count() * 8 +   // stts box size
																													mCttsTableEntries->count() * 8 +   // ctts box size
																													stcoBoxSizeBytes +           // stco box size
																													stszBoxSizeBytes;            // stsz box size
																				#ifndef ANDROID_DEFAULT_CODE
																						mEstimatedTrackSizeBytes += TRACK_HEADER_RESERVE_BYTES;   //added by hai.li @2010-12-25 to check file size limit accurately

//判断大小																						}
if (mOwner->exceedsFileSizeLimit() && (mStszTableEntries->count() != 0))
		// 发送在基类MediaWriter. App收到消息发送stop, stop最终都由app处理
		mOwner->notify(MEDIA_RECORDER_EVENT_INFO, MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED, 0);

//退出循环后调用bufferChunk
//从Log看到，有时是一次，有时是二次。
D:\Downloads\Console-2.00b148-Beta_32bit\Console2>adb logcat -v time -s StagefrightRecorder MPEG4Writer | grep -E "4664"
01-07 21:26:39.941 W/MPEG4Writer( 7052): [threadEntry],[4664] call bufferChunk
01-07 21:26:40.037 W/MPEG4Writer( 7052): [threadEntry],[4664] call bufferChunk
01-07 21:26:49.765 W/MPEG4Writer( 7052): [threadEntry],[4664] call bufferChunk
01-07 21:27:01.474 W/MPEG4Writer( 7052): [threadEntry],[4664] call bufferChunk
01-07 21:27:10.688 W/MPEG4Writer( 7052): [threadEntry],[4664] call bufferChunk
01-07 21:27:21.870 W/MPEG4Writer( 7052): [threadEntry],[4664] call bufferChunk
01-07 21:27:21.965 W/MPEG4Writer( 7052): [threadEntry],[4664] call bufferChunk
01-07 21:27:31.955 W/MPEG4Writer( 7052): [threadEntry],[4664] call bufferChunk

==============            
读到的第一帧是config,会执行continue
#ifndef ANDROID_DEFAULT_CODE
        if (buffer->meta_data()->findInt32(kKeyIsCodecConfig, &isCodecConfig)
#ifdef MTK_VIDEO_HEVC_SUPPORT
			&& isCodecConfig && (mIsAvc || mIsMPEG4 || mIsHevc)) {//only AVC/MPEG4/AAC/HEVC has codec info
#else
			&& isCodecConfig && (mIsAvc || mIsMPEG4)) {//only AVC/MPEG4/AAC has codec info
#endif
#else
        if (buffer->meta_data()->findInt32(kKeyIsCodecConfig, &isCodecConfig)
                && isCodecConfig) {
#endif

			mGotAllCodecSpecificData = true;
            ALOGD("[%s], [%d] , mOwner->isMinor():%d ,mIsAudio:%d continue",__FUNCTION__, __LINE__, mOwner->isMinor(), mIsAudio);

            continue;