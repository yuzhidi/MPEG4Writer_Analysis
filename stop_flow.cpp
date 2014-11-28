
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
																					
																					
																					
																					�ļ���С�ﵽ���޵�ֹͣ exceedsFileSizeLimit()
																					
																					//resume��pauseĿǰ��������
																					signalResumed(hasMultipleTracks);
																					mPauseCondition.signal();
																					
																					
																					
																					

																					
//�ж��ļ���С���߼�:
		//ÿ��ȡһ��sample,��Ҫ����mMdatSizeBytes,
        mMdatSizeBytes += sampleSize;
		//ͬʱ����moov�ṹ�Ĵ�С
        updateTrackSizeEstimate();												estimate�����ƣ�Ԥ��
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

//�жϴ�С																						}
if (mOwner->exceedsFileSizeLimit() && (mStszTableEntries->count() != 0))
		// �����ڻ���MediaWriter. App�յ���Ϣ����stop, stop���ն���app����
		mOwner->notify(MEDIA_RECORDER_EVENT_INFO, MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED, 0);

//�˳�ѭ�������bufferChunk
//��Log��������ʱ��һ�Σ���ʱ�Ƕ��Ρ�
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
�����ĵ�һ֡��config,��ִ��continue
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