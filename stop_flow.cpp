
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