//   Copyright 2015-2016 Ansersion
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

#include <jrtplib3\rtcppacket.h>
#include <iostream>
#ifdef _WIN32
#pragma comment(lib, "jrtplib.lib")
#pragma comment(lib, "jthread.lib")
#endif

#include "rtspClient.h"
#include "utils.h"
#include "Base64.hh"
#include "nalu_types_h264.h"
#include "mpeg_types.h"
#include "pcmu_types.h"

#include <sstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <strings.h>
#endif
#define GET_SPS_PPS_PERIOD 	30

using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;

extern STAP_A 		STAP_AObj;
extern STAP_B 		STAP_BObj;
extern MTAP_16 		MTAP_16Obj;
extern MTAP_24 		MTAP_24Obj;
extern FU_A 		FU_AObj;
extern FU_B 		FU_BObj;
extern NALUTypeBase_H264 NaluBaseType_H264Obj;

extern MPEG_Audio MPEG_AudioObj;
extern PCMU_Audio PCMU_AudioObj;

extern NALUTypeBase_H265 NaluBaseType_H265Obj;

#define MEDIA_BUFSIZ 8192

RtspClient::RtspClient():
	RtspURI(""), RtspCSeq(0), RtspSockfd(-1), RtspIP(""), RtspPort(PORT_RTSP), RtspResponse(""), SDPStr(""), 
	VPS(""), SPS(""), PPS(""), CmdPLAYSent(false), GetVideoDataCount(GET_SPS_PPS_PERIOD),
	Username(""), Password(""), Realm(""), Nonce("")
{
#ifndef _WIN32

#else
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	// SDPInfo = new multimap<string, string>;
	MediaSessionMap = new map<string, MediaSession>;
	AudioBuffer.Size = 0;
	VideoBuffer.Size = 0;
	if((AudioBuffer.Buf = (uint8_t *)malloc(MEDIA_BUFSIZ)))
		AudioBuffer.Size = MEDIA_BUFSIZ;
	if((VideoBuffer.Buf = (uint8_t *)malloc(MEDIA_BUFSIZ)))
		VideoBuffer.Size = MEDIA_BUFSIZ;

	ByeFromServerAudioClbk = NULL;
	ByeFromServerVideoClbk = NULL;

	/* Temporary only FU_A supported */
	// NALUType = new FU_A;
    ObtainVpsSpsPpsPeriodly = true;
}

RtspClient::RtspClient(string uri):
	RtspURI(uri), RtspCSeq(0), RtspSockfd(-1), RtspIP(""), RtspPort(PORT_RTSP), RtspResponse(""), SDPStr(""),
	VPS(""), SPS(""), PPS(""), CmdPLAYSent(false), GetVideoDataCount(GET_SPS_PPS_PERIOD)
{
#ifndef _WIN32

#else
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	// SDPInfo = new multimap<string, string>;
	MediaSessionMap = new map<string, MediaSession>;
	AudioBuffer.Size = 0;
	VideoBuffer.Size = 0;
	if((AudioBuffer.Buf = (uint8_t *)malloc(MEDIA_BUFSIZ)))
		AudioBuffer.Size = MEDIA_BUFSIZ;
	if((VideoBuffer.Buf = (uint8_t *)malloc(MEDIA_BUFSIZ)))
		VideoBuffer.Size = MEDIA_BUFSIZ;

	/* Temporary only FU_A supported */
	// NALUType = new FU_A;
	ByeFromServerAudioClbk = NULL;
	ByeFromServerVideoClbk = NULL;

    ObtainVpsSpsPpsPeriodly = true;
}

RtspClient::~RtspClient()
{
	// delete SDPInfo;
	delete MediaSessionMap;
	MediaSessionMap = NULL;

	if(AudioBuffer.Buf) {
		free(AudioBuffer.Buf);
		AudioBuffer.Buf = NULL;
		AudioBuffer.Size = 0;
	}

	if(VideoBuffer.Buf) {
		free(VideoBuffer.Buf);
		VideoBuffer.Buf = NULL;
		VideoBuffer.Size = 0;
	}
#ifndef _WIN32

#else
	WSACleanup();
#endif
	// delete NALUType;
	// NALUType = NULL;
}

ErrorType RtspClient::DoDESCRIBE(string uri)
{
	string RtspUri("");
	int Sockfd = -1;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return RTSP_INVALID_URI;

	Sockfd = CreateTcpSockfd(RtspUri);
	if(Sockfd < 0) return RTSP_INVALID_URI;

	string Cmd("DESCRIBE");
	stringstream Msg("");
	Msg << Cmd << " " << RtspUri << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "\r\n";

	if(!SendRTSP(Sockfd, Msg.str())) {
		// close(Sockfd);
		Close(Sockfd);
		return RTSP_SEND_ERROR;
	}
	if(!RecvRTSP(Sockfd, &RtspResponse)) {
		Close(Sockfd);
		// close(Sockfd);
		return RTSP_RECV_ERROR;
	}
	// check username and password, if any
	if(CheckAuth(Sockfd, Cmd, RtspUri) != CHECK_OK) {
		cout << "CheckAuth: error" << endl;
		// close(Sockfd);
		Close(Sockfd);
		return RTSP_RESPONSE_401;
	}
	RecvSDP(Sockfd, &SDPStr);
	// close(Sockfd);
	return RTSP_NO_ERROR;
}

ErrorType RtspClient::DoOPTIONS(string uri)
{
	string RtspUri("");
	int Sockfd = -1;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return RTSP_INVALID_URI;

	Sockfd = CreateTcpSockfd(RtspUri);
	if(Sockfd < 0) return RTSP_INVALID_URI;

	string Cmd("OPTIONS");
	stringstream Msg("");
	Msg << Cmd << " " << RtspUri << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "\r\n";

	if(!SendRTSP(Sockfd, Msg.str())) {
		// close(Sockfd);
		Close(Sockfd);
		return RTSP_SEND_ERROR;
	}
	if(!RecvRTSP(Sockfd, &RtspResponse)) {
		// close(Sockfd);
		Close(Sockfd);
		return RTSP_RECV_ERROR;
	}
	// close(Sockfd);
	return RTSP_NO_ERROR;
}

ErrorType RtspClient::DoPAUSE()
{
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		Err = DoPAUSE(&(it->second));
		if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
		printf("PAUSE Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	}

	return ErrAll;
}

ErrorType RtspClient::DoPAUSE(MediaSession * media_session)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
	}
	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;

	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;
	
	string Cmd("PAUSE");
	stringstream Msg("");
	Msg << Cmd << " " << RtspURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "Session: " << media_session->SessionID << "\r\n";
	Msg << "\r\n";

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		Close(Sockfd);
		// close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		Close(Sockfd);
		// close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	// close(Sockfd);
	return Err;
}

ErrorType RtspClient::DoPAUSE(string media_type)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoPAUSE(&(it->second));
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

ErrorType RtspClient::DoGET_PARAMETER()
{
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		Err = DoGET_PARAMETER(&(it->second));
		if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
		printf("GET_PARAMETER Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	}

	return ErrAll;
}

ErrorType RtspClient::DoGET_PARAMETER(MediaSession * media_session)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
	}
	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;

	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;
	
	string Cmd("GET_PARAMETER");
	stringstream Msg("");
	Msg << Cmd << " " << RtspURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "Session: " << media_session->SessionID << "\r\n";
	Msg << "\r\n";

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		Close(Sockfd);
		// close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		Close(Sockfd);
		// close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	// close(Sockfd);
	return Err;
}

ErrorType RtspClient::DoGET_PARAMETER(string media_type)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoGET_PARAMETER(&(it->second));
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

ErrorType RtspClient::DoSETUP()
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		Err = DoSETUP(&(it->second));
		if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
		printf("Setup Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	}

	return ErrAll;
}

ErrorType RtspClient::DoSETUP(MediaSession * media_session)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
	}
	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;

	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;
	
	// "CreateUdpSockfd" is only for test. 
	// We will use jrtplib instead later. 
	if(!SetAvailableRTPPort(media_session)) {
		printf("No port available for RTP and RTCP\n");
		return RTSP_UNKNOWN_ERROR;
	}
	string Cmd("SETUP");
	stringstream Msg("");
	Msg << Cmd << " " << media_session->ControlURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "Transport:" << " " << media_session->Protocol << "/UDP;";
	Msg << "unicast;" << "client_port=" << media_session->RTPPort << "-" << media_session->RTCPPort << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	if(Realm.length() > 0 && Nonce.length() > 0) {
		string RealmTmp = Realm;
		string NonceTmp = Nonce;
		string Md5Response  = MakeMd5DigestResp(RealmTmp, Cmd, media_session->ControlURI, NonceTmp);
		if(Md5Response.length() != MD5_SIZE) {
			cout << "Make MD5 digest response error" << endl;
			return RTSP_RESPONSE_401;
		}
		Msg << "Authorization: Digest username=\"" << Username << "\", realm=\""
			<< RealmTmp << "\", nonce=\"" << NonceTmp << "\", uri=\"" << media_session->ControlURI
			<< "\", response=\"" << Md5Response << "\"\r\n";
	}
	Msg << "\r\n";

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		// close(Sockfd);
		Close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		// close(Sockfd);
		Close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	// if(CheckAuth(Sockfd, Cmd, media_session->ControlURI) != CHECK_OK) {
	// 	cout << "CheckAuth: error" << endl;
	// 	close(Sockfd);
	// 	return RTSP_RESPONSE_401;
	// }
	media_session->SessionID = ParseSessionID(RtspResponse);
	int timeout = ParseTimeout(RtspResponse);
	if(timeout <= 0) {
		// default 60
		media_session->Timeout = 60;
	}
	media_session->Timeout = timeout;

	media_session->RTP_SetUp();
	SetDestroiedClbk("audio", ByeFromServerAudioClbk);
	SetDestroiedClbk("video", ByeFromServerVideoClbk);

	// media_session->RTSPSockfd = Sockfd;
	// close(Sockfd);
	return Err;
}

ErrorType RtspClient::DoSETUP(string media_type)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;


	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoSETUP(&(it->second));
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

ErrorType RtspClient::DoPLAY()
{
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		Err = DoPLAY(&(it->second));
		if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
		printf("PLAY Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	}

	return ErrAll;
}

ErrorType RtspClient::DoPLAY(MediaSession * media_session, float * scale, float * start_time, float * end_time)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
	}

	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;
	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;

	string Cmd("PLAY");
	stringstream Msg("");
	Msg << Cmd << " " << RtspURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	if(scale) {
		char floatChar[32];
		sprintf(floatChar, "%.1f", *scale);
		Msg << "Scale: " << floatChar << "\r\n";
	}
	if(start_time || end_time) {
		char floatChar[32];
		Msg << "Range: npt=";
		if(start_time) {
			sprintf(floatChar, "%.1f", *start_time);
			Msg << floatChar;
		}
		Msg << "-";
		if(end_time) {
			sprintf(floatChar, "%.1f", *end_time);
			Msg << floatChar;
		}
		Msg << "\r\n";
	}
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "Session: " << media_session->SessionID << "\r\n";
	if(Realm.length() > 0 && Nonce.length() > 0) {
		string RealmTmp = Realm;
		string NonceTmp = Nonce;
		string Md5Response  = MakeMd5DigestResp(RealmTmp, Cmd, RtspURI,  NonceTmp);
		if(Md5Response.length() != MD5_SIZE) {
			cout << "Make MD5 digest response error" << endl;
			return RTSP_RESPONSE_401;
		}
		Msg << "Authorization: Digest username=\"" << Username << "\", realm=\""
			<< RealmTmp << "\", nonce=\"" << NonceTmp << "\", uri=\"" << RtspURI
			<< "\", response=\"" << Md5Response << "\"\r\n";
	}
	Msg << "\r\n";
	std::cout << Msg.str();

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		Close(Sockfd);
		// close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		Close(Sockfd);
		// close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	// if(CheckAuth(Sockfd, Cmd, RtspURI) != CHECK_OK) {
	// 	cout << "CheckAuth: error" << endl;
	// 	close(Sockfd);
	// 	return RTSP_RESPONSE_401;
	// }
	// close(Sockfd);
	return Err;
}

ErrorType RtspClient::DoPLAY(string media_type, float * scale, float * start_time, float * end_time)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoPLAY(&(it->second), scale, start_time, end_time);
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

ErrorType RtspClient::DoTEARDOWN()
{
	ErrorType Err = RTSP_NO_ERROR;
	ErrorType ErrAll = RTSP_NO_ERROR;

	Err = DoTEARDOWN("audio");
	if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
	Err = DoTEARDOWN("video");
	if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error

	// for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
	// 	cerr << "TEST: Ready to teardown: " << it->first << endl;
	// 	Err = DoTEARDOWN(&(it->second));
	// 	if(RTSP_NO_ERROR == ErrAll) ErrAll = Err; // Remeber the first error
	// 	printf("TEARDOWN Session %s: %s\n", it->first.c_str(), ParseError(Err).c_str());
	// }

	return ErrAll;
}

ErrorType RtspClient::DoTEARDOWN(MediaSession * media_session)
{
	if(!media_session) {
		return RTSP_INVALID_MEDIA_SESSION;
		// return RTSP_NO_ERROR;
	}
	ErrorType Err = RTSP_NO_ERROR;
	int Sockfd = -1;

	cout << "TEST: TEARDOWN: ###" << media_session->MediaType << "###" << endl;

	Sockfd = CreateTcpSockfd();
	if(Sockfd < 0) return RTSP_INVALID_URI;
	
	string Cmd("TEARDOWN");
	stringstream Msg("");
	Msg << Cmd << " " << RtspURI << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "Session: " << media_session->SessionID << "\r\n";
	if(Realm.length() > 0 && Nonce.length() > 0) {
		string RealmTmp = Realm;
		string NonceTmp = Nonce;
		string Md5Response  = MakeMd5DigestResp(RealmTmp, Cmd, RtspURI,  NonceTmp);
		if(Md5Response.length() != MD5_SIZE) {
			cout << "Make MD5 digest response error" << endl;
			return RTSP_RESPONSE_401;
		}
		Msg << "Authorization: Digest username=\"" << Username << "\", realm=\""
			<< RealmTmp << "\", nonce=\"" << NonceTmp << "\", uri=\"" << RtspURI
			<< "\", response=\"" << Md5Response << "\"\r\n";
	}
	Msg << "\r\n";

	if(RTSP_NO_ERROR == Err && !SendRTSP(Sockfd, Msg.str())) {
		// close(Sockfd);
		Close(Sockfd);
		Sockfd = -1;
		Err = RTSP_SEND_ERROR;
	}
	if(RTSP_NO_ERROR == Err && !RecvRTSP(Sockfd, &RtspResponse)) {
		// close(Sockfd);
		Close(Sockfd);
		Sockfd = -1;
		Err = RTSP_RECV_ERROR;
	}
	if(RTSP_NO_ERROR == Err) {
		map<string, MediaSession>::iterator it;
		for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
			if(media_session->SessionID == it->second.SessionID) break;
		}
		if(it != MediaSessionMap->end()) {
			MediaSessionMap->erase(it);
			// close(media_session->RTPSockfd);
			// close(media_session->RTCPSockfd);
		}
	}
	Close(Sockfd);
	return Err;
}

ErrorType RtspClient::DoTEARDOWN(string media_type)
{
	MyRegex Regex;
	ErrorType Err = RTSP_NO_ERROR;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it != MediaSessionMap->end()) {
		Err = DoTEARDOWN(&(it->second));
		return Err;
	}
	Err = RTSP_INVALID_MEDIA_SESSION;
	return Err;
}

int RtspClient::ParseSDP(string SDP)
{
	MyRegex Regex;
	string Response("");
	int Result = 0; // don't have meaning yet

	if(SDP.length() != 0) Response.assign(SDP);
	else if(RtspResponse.length() != 0) Response.assign(SDPStr);
	else return Result;

	string Pattern = "([a-zA-Z])=(.*)";
	list<string> Group;
	bool CollectMediaInfo = false;
	string CurrentMediaSession("");
	while(Regex.RegexLine(&Response, &Pattern, &Group)) {
		string Key(""), Value("");
		if(!Group.empty()) {
			Group.pop_front(); // pop the line
			Group.pop_front(); // pop the matched part
			Key.assign(Group.front()); Group.pop_front();
			Value.assign(Group.front()); Group.pop_front();
			// SDPInfo->insert(pair<string, string>(Key, Value));
		}
		if(Key == "m") CollectMediaInfo = true;
		if(Key == "s") CollectMediaInfo = false;
		if(!CollectMediaInfo) continue;

		if(Key == "m") { 
            /* Pattern: (MediaType) +(Ports) +(Protocol) +(PayloadType)"
               Example: "(audio) (0) (RTP/AVP) (14)"
			   */
			// string PatternTmp("([a-zA-Z]+) +.+ +(.+) +.*");
			// string PatternTmp("([a-zA-Z]+) +([0-9/]+) +([A-Za-z/]+) +\\b([0-9]+)\\b");
			string PatternTmp("([a-zA-Z]+) +([0-9/]+) +([A-Za-z/]+) +([0-9]+)");
			if(!Regex.Regex(Value.c_str(), PatternTmp.c_str(), &Group)) {
				continue;
			}
			Group.pop_front();
			CurrentMediaSession.assign(Group.front());
			Group.pop_front();
			Group.pop_front(); // FIXME: Ports are ignored
			string Protocol(Group.front());
			Group.pop_front();
			int PayloadTypeTmp = -1;
			stringstream ssPayloadType;
			ssPayloadType << Group.front();
			ssPayloadType >> PayloadTypeTmp;

			MediaSession NewMediaSession;
			NewMediaSession.MediaType.assign(CurrentMediaSession);
			NewMediaSession.Protocol.assign(Protocol);
			NewMediaSession.PayloadType.push_back(PayloadTypeTmp);
			(*MediaSessionMap)[CurrentMediaSession] = NewMediaSession;

		}
		if("a" == Key) {
			// string PatternRtpmap("rtpmap:.* +([0-9A-Za-z]+)/([0-9]+)");
			string PatternRtpmap("rtpmap:.* +([0-9A-Za-z]+)/([0-9]+)/?([0-9])?");
			string PatternFmtp_H264("fmtp:.*sprop-parameter-sets=([A-Za-z0-9+/=]+),([A-Za-z0-9+/=]+)");
			string PatternFmtp_H265("fmtp:.*sprop-vps=([A-Za-z0-9+/=]+);.*sprop-sps=([A-Za-z0-9+/=]+);.*sprop-pps=([A-Za-z0-9+/=]+)");
			string PatternControl("control:(.+)");
			if(CurrentMediaSession.length() == 0) {
				continue;
			}
			if(Regex.Regex(Value.c_str(), PatternRtpmap.c_str(), &Group)) {
				Group.pop_front();
				(*MediaSessionMap)[CurrentMediaSession].EncodeType = Group.front();;
				Group.pop_front();
				stringstream TimeRate;
				TimeRate << Group.front();
				TimeRate >> (*MediaSessionMap)[CurrentMediaSession].TimeRate;
                Group.pop_front();
                if(!Group.empty()) {
                    stringstream ChannelNum;
                    ChannelNum << Group.front();
                    ChannelNum >> (*MediaSessionMap)[CurrentMediaSession].ChannelNum;
                }

			} else if(Regex.Regex(Value.c_str(), PatternControl.c_str(), &Group)) {
				Group.pop_front();
				string ControlURITmp("");
				/* 'Value' could be  
				 * 1: "rtsp://127.0.0.1/ansersion/track=1"
				 * 2: "track=1"
				 * If is the '2', it should be prefixed with the URI. */
				if(!Regex.Regex(Value.c_str(), "rtsp://")) {
					ControlURITmp += RtspURI;
					ControlURITmp += "/";
				}
				ControlURITmp += Group.front();
				printf("Control: %s\n", ControlURITmp.c_str());
				(*MediaSessionMap)[CurrentMediaSession].ControlURI.assign(ControlURITmp);
			} else if(Regex.Regex(Value.c_str(), PatternFmtp_H264.c_str(), &Group)) {
				Group.pop_front();
				SPS.assign(Group.front());
				Group.pop_front();
				PPS.assign(Group.front());

				if(Regex.Regex(Value.c_str(), "packetization-mode=([0-2])", &Group)) {
					Group.pop_front();
					stringstream PacketizationMode;
					PacketizationMode << Group.front();
					PacketizationMode >> (*MediaSessionMap)[CurrentMediaSession].Packetization;
				}
			} else if(Regex.Regex(Value.c_str(), PatternFmtp_H265.c_str(), &Group)) {
				Group.pop_front();
				VPS.assign(Group.front());
				Group.pop_front();
				SPS.assign(Group.front());
				Group.pop_front();
				PPS.assign(Group.front());
			}
		}
	}
	
	for(map<string, MediaSession>::iterator it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		it->second.MediaInfoCheck();
	}

	return Result;
}

string RtspClient::ParseError(ErrorType et)
{
	string ErrStr("");
	switch(et) {
		case RTSP_NO_ERROR:
			ErrStr.assign("MyRtsp: Success");
			break;
		case RTSP_INVALID_URI:
			ErrStr.assign("MyRtsp: Invalid URI");
			break;
		case RTSP_SEND_ERROR:
			ErrStr.assign("MyRtsp: send error");
			break;
		case RTSP_RECV_ERROR:
			ErrStr.assign("MyRtsp: recv error");
			break;
		case RTSP_INVALID_MEDIA_SESSION:
			ErrStr.assign("MyRtsp: invalid media session error");
			break;
		case RTSP_RESPONSE_BLANK:
			ErrStr.assign("MyRtsp: Response BLANK");
			break;
		case RTSP_RESPONSE_200:
			ErrStr.assign("MyRtsp: Response 200 OK");
			break;
		case RTSP_RESPONSE_400:
			ErrStr.assign("MyRtsp: Response 400 Bad Request");
			break;
		case RTSP_RESPONSE_401:
			ErrStr.assign("MyRtsp: Response 401 Unauthorized");
			break;
		case RTSP_RESPONSE_404:
			ErrStr.assign("MyRtsp: Response 404 Not Found");
			break;
		case RTSP_RESPONSE_40X:
			ErrStr.assign("MyRtsp: Response Client Error");
			break;
		case RTSP_RESPONSE_500:
			ErrStr.assign("MyRtsp: Response 500 Internal Server Error");
			break;
		case RTSP_RESPONSE_501:
			ErrStr.assign("MyRtsp: Response 501 Not Implemented");
			break;
		case RTSP_RESPONSE_50X:
			ErrStr.assign("MyRtsp: Response Server Error");
			break;
		case RTSP_UNKNOWN_ERROR:
			ErrStr.assign("MyRtsp: Unknown Error");
			break;
		default:
			ErrStr.assign("MyRtsp: Unknown Error");
			break;
	}
	return ErrStr;
}

/*****************/
/* Tools Methods */
int RtspClient::CreateTcpSockfd(string uri)
{
#ifndef _WIN32
	int Sockfd = -1;
#else
	SOCKET Sockfd = -1;
#endif
	struct sockaddr_in Servaddr;
	string RtspUri("");
	int SockStatus = -1;

	if(RtspSockfd > 0) {
		return RtspSockfd;
	}

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else {
		RtspSockfd = Sockfd;
		return Sockfd;
	}

#ifndef _WIN32
	if((Sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("Socket created error");
		RtspSockfd = Sockfd;
		return Sockfd;
	}
	else
	{
		printf("create success\n");
	}
#else
	if ((Sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket created error");
		RtspSockfd = Sockfd;
		return Sockfd;
	}
	else
	{
		printf("create success\n");
	}
#endif

	// Set Sockfd NONBLOCK
#ifndef _WIN32
	SockStatus = fcntl(Sockfd, F_GETFL, 0);
	fcntl(Sockfd, F_SETFL, SockStatus | O_NONBLOCK);
	// Connect to server
	bzero(&Servaddr, sizeof(Servaddr));
	// Servaddr.sin_len = sizeof(struct sockaddr_in);
	Servaddr.sin_family = AF_INET;
	Servaddr.sin_port = htons(GetPort(uri));
	Servaddr.sin_addr.s_addr = GetIP(uri);
#else
	// ����socketΪ������  
	u_long imode = 1; // 0Ϊ��������0Ϊ������  
	//���÷Ƕ�������ʧ�ܲ�֪��ԭ��
	//ioctlsocket(Sockfd, FIONBIO, &imode);
	memset(&Servaddr, '\0', sizeof(Servaddr));
	// Servaddr.sin_len = sizeof(struct sockaddr_in);
	Servaddr.sin_family = AF_INET;
	Servaddr.sin_port = htons(GetPort(uri));
	Servaddr.sin_addr.S_un.S_addr = GetIP(uri);
#endif


	if(connect(Sockfd, (struct sockaddr *)&Servaddr, sizeof(sockaddr)) < 0 && errno != EINPROGRESS) {
		perror("connect error");
#ifndef _WIN32
		close(Sockfd);
#else
		closesocket(Sockfd);
#endif
		Sockfd = -1;
		RtspSockfd = Sockfd;
		return Sockfd;
	}
	if(!CheckSockWritable(Sockfd)) {
		Sockfd = -1;
		RtspSockfd = Sockfd;
		return Sockfd;
	}

	RtspSockfd = Sockfd;
	return Sockfd;
}

int RtspClient::SetAvailableRTPPort(MediaSession * media_session, uint16_t RTP_port)
{
	int RTPSockfd, RTCPSockfd;
	uint16_t RTPPort;
	uint16_t RTCPPort;
	struct sockaddr_in servaddr;
	uint16_t Search_RTP_Port_From;
	if(0 != RTP_port && (RTP_port % 2 == 0)) {
		Search_RTP_Port_From = RTP_port;
	} else {
		Search_RTP_Port_From = SEARCH_PORT_RTP_FROM;
	}
	media_session->RTPPort = 0;
	// media_session->RTPSockfd = -1;
	media_session->RTCPPort = 0;
	// media_session->RTCPSockfd = -1;

	// Create RTP and RTCP udp socket 
	for(RTPPort = Search_RTP_Port_From; RTPPort < 65535; RTPPort = RTPPort + 2) {
		// Bind RTP Port
		if((RTPSockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("CreateRTP_RTCPSockfd Error");
			return 0;
		}	
		// if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) < 0) { 
		// 	close(RTPPort);
		// 	perror("CreateRTP_RTCPSockfd Error");
		// 	return 0;
		// } 
#ifndef _WIN32
		bzero(&servaddr, sizeof(servaddr));
#else
		//bzero(&servaddr, sizeof(servaddr));
#endif
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(RTPPort);
		if(bind(RTPSockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
#ifndef _WIN32
			close(RTPSockfd);
#else
			closesocket(RTPSockfd);
#endif
			continue;
		}

		// Bind RTCP Port
		RTCPPort = RTPPort + 1;
		if((RTCPSockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
#ifndef _WIN32
			close(RTPSockfd);
#else
			closesocket(RTPSockfd);
#endif
			perror("CreateRTP_RTCPSockfd Error");
			return 0; // Create failed
		}	
		// if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) < 0) { 
		// 	close(RTPPort);
		// 	close(RTCPPort);
		// 	perror("CreateRTP_RTCPSockfd Error");
		// 	return 0;
		// } 
#ifndef _WIN32
		bzero(&servaddr, sizeof(servaddr));
#else

#endif
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(RTCPPort);

		if(bind(RTCPSockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
#ifndef _WIN32
			close(RTPSockfd);
			close(RTCPSockfd);
#else
			closesocket(RTPSockfd);
			closesocket(RTCPSockfd);
#endif
			
			continue;
		}
#ifndef _WIN32
		close(RTPSockfd);
		close(RTCPSockfd);
#else
		closesocket(RTPSockfd);
		closesocket(RTCPSockfd);
#endif
		media_session->RTPPort = RTPPort;
		// media_session->RTPSockfd = RTPSockfd;;
		media_session->RTCPPort = RTCPPort;
		// media_session->RTCPSockfd = RTCPSockfd;
		return 1; // Created successfully
	}
	return 0; // Created failed
}

#ifndef _WIN32
in_addr_t RtspClient::GetIP(string uri)
#else
ULONG RtspClient::GetIP(string uri)
#endif
{
	//### example uri: rtsp://192.168.15.100/test ###//
	MyRegex Regex;
	string RtspUri("");
	// string Pattern("rtsp://([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})");
	string Pattern("rtsp://([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})/[^0-9]");
	// string Pattern("rtsp://(192.168.1.143)");
	list<string> Groups;
	bool IgnoreCase = true;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return inet_addr(RtspIP.c_str());

	if(!Regex.Regex(RtspUri.c_str(), Pattern.c_str(), &Groups, IgnoreCase)) {
		Pattern = "rtsp://([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}):[0-9]";
		if (!Regex.Regex(RtspUri.c_str(), Pattern.c_str(), &Groups, IgnoreCase))
		{
			return INADDR_NONE;
		}
		else
		{
			Groups.pop_front();
			return inet_addr(Groups.front().c_str());
		}
		return INADDR_NONE;
	}
	Groups.pop_front();
	return inet_addr(Groups.front().c_str());
}

uint16_t RtspClient::GetPort(string uri)
{
	//### example uri: rtsp://192.168.15.100:554/test ###//
	MyRegex Regex;
	string RtspUri("");
	string Pattern("rtsp://[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}:([0-9]+)");
	list<string> Groups;
	bool IgnoreCase = true;

	if(uri.length() != 0) {
		RtspUri.assign(uri);
		RtspURI.assign(uri);
	}
	else if(RtspURI.length() != 0) RtspUri.assign(RtspURI);
	else return RtspPort;

	if(!Regex.Regex(RtspUri.c_str(), Pattern.c_str(), &Groups, IgnoreCase)) {
		return RtspPort;
	}
	Groups.pop_front();
	return atoi(Groups.front().c_str());
}

/*********************/
/* Protected Methods */
int RtspClient::CheckSockWritable(int sockfd, struct timeval * tval)
{
	fd_set Wset;
	struct timeval Tval;
	FD_ZERO(&Wset);
	FD_SET(sockfd, &Wset);
	if(!tval) {
		Tval.tv_sec = SELECT_TIMEOUT_SEC;
		Tval.tv_usec = SELECT_TIMEOUT_USEC;
	} else {
		Tval = *tval;
	}

	while(select(sockfd + 1, NULL, &Wset, NULL, &Tval) != 0) {
		if(FD_ISSET(sockfd, &Wset)) {
			return CHECK_OK;
		}
		return CHECK_ERROR;
	}   
	printf("Select Timeout\n");
	return CHECK_ERROR;
}

int RtspClient::CheckSockReadable(int sockfd, struct timeval * tval)
{
	fd_set Rset;
	struct timeval Tval;
	FD_ZERO(&Rset);
	FD_SET(sockfd, &Rset);
	if(!tval) {
		Tval.tv_sec = SELECT_TIMEOUT_SEC;
		Tval.tv_usec = SELECT_TIMEOUT_USEC;
	} else {
		Tval = *tval;
	}

	while(select(sockfd + 1, &Rset, NULL, NULL, &Tval) != 0) {
		if(FD_ISSET(sockfd, &Rset)) {
			return CHECK_OK;
		}
		return CHECK_ERROR;
	}   
	printf("Select Timeout\n");
	return CHECK_ERROR;
}

int RtspClient::SendRTSP(int fd, const char * msg, size_t size)
{
 	if(!msg || size < 0) {
 		printf("Recv Argument Error\n");
 		return TRANS_ERROR;
 	}

 	int SendResult = 0;
 	int Index = 0;
 	int Err = TRANS_OK;

	while(size > 0) {
		if(!CheckSockWritable(fd)) {
			Err = TRANS_ERROR;
			break;
		}
		SendResult = Writen(fd, msg+Index, size);
		if(SendResult < 0) {
			if(errno == EINTR) continue;
			else if(errno == EWOULDBLOCK || errno == EAGAIN) continue;
			else {
				Err = TRANS_ERROR;
				break;
			}
		} else if(SendResult == 0) {
			Err = TRANS_ERROR;
			break;
		}
		Index += SendResult;
		size -= SendResult;
	}

	return Err;
}
// {
// 	if(!msg || size <= 0) {
// 		printf("Recv Argument Error\n");
// 		return TRANS_ERROR;
// 	}
// 
// 	int SendResult = 0;
// 	int Index = 0;
// 	int Err = TRANS_OK;
// 	while(size != 0) {
// 		if(!CheckSockWritable(RtspSockfd)) {
// 			Err = TRANS_ERROR;
// 			break;
// 		}
// 		SendResult = send(RtspSockfd, msg+Index, size, 0);
// 		if(0 == SendResult) {
// 			printf("Socket disconnected\n");
// 			Err = TRANS_ERROR;
// 			break;
// 		} else if(SendResult < 0) {
// 			if(errno == EINTR) continue;
// 			else if(errno == EWOULDBLOCK || errno == EAGAIN) {
// 				Err = TRANS_OK;
// 				break;
// 			} else {
// 				Err = TRANS_ERROR;
// 				break;
// 			}
// 		} else {
// 			Index += SendResult;
// 			size -= SendResult;
// 			continue;
// 		}
// 	}
// 
// 	return Err;
// }

int RtspClient::SendRTSP(int fd, string msg)
{
	return SendRTSP(fd, msg.c_str(), msg.length());
}

int RtspClient::RecvRTSP(int fd, char * msg, size_t maxlen)
{
	MyRegex Regex;
	if(!msg || maxlen < 0) {
		printf("Recv Argument Error\n");
		return TRANS_ERROR;
	}

	int RecvResult = 0;
	int Index = 0;
	int Err = TRANS_OK;

	memset(msg, 0, maxlen);
	while(maxlen > 0) {
		if(!CheckSockReadable(fd)) {
			Err = TRANS_ERROR;
			break;
		}
		RecvResult = ReadLine(fd, msg + Index, maxlen);
		if(RecvResult < 0) {
			if(errno == EINTR) continue;
			else if(errno == EWOULDBLOCK || errno == EAGAIN) {
				Err = TRANS_OK;
				break;
			} else {
				Err = TRANS_ERROR;
				break;
			}
		} else if(RecvResult == 0) {
			Err = TRANS_ERROR;
			break;
		}
		/*
		 * Single with "\r\n" or "\n" is the termination tag of RTSP.
		 * */
		if(RecvResult <= (int)strlen("\r\n") &&
				Regex.Regex(msg+Index, "^(\r\n|\n)$")) {
			Err = TRANS_OK;
			break;
		}
		Index += RecvResult;
		maxlen -= RecvResult;
	}
	return Err;
}

int RtspClient::RecvRTSP(int fd, string * msg)
{
	if(!msg) {
		printf("Invalid Argument\n");
		return TRANS_ERROR;
	}

	char * Buf = (char *)calloc(RECV_BUF_SIZE, sizeof(char));
	int RecvResult = TRANS_OK;

	msg->assign("");
	RecvResult = RecvRTSP(fd, Buf, RECV_BUF_SIZE);
	if(TRANS_OK == RecvResult) msg->assign(Buf);
	free(Buf);
	return RecvResult;
}

int RtspClient::RecvSDP(int sockfd, char * msg, size_t size)
{
	if(!msg || size < 0) {
		printf("Recv Argument Error\n");
		return TRANS_ERROR;
	}

	int RecvResult = 0;
	int Index = 0;
	int Err = TRANS_OK;

	memset(msg, 0, size);
	while(size > 0) {
		if(!CheckSockReadable(sockfd)) {
			Err = TRANS_ERROR;
			break;
		}
		RecvResult = Readn(sockfd, msg + Index, size);
		if(RecvResult < 0) {
			if(errno == EINTR) continue;
			else if(errno == EWOULDBLOCK || errno == EAGAIN) {
				Err = TRANS_OK;
				break;
			} else {
				Err = TRANS_ERROR;
				break;
			}
		} else if(RecvResult == 0) {
			Err = TRANS_ERROR;
			break;
		}
		Index += RecvResult;
		size -= RecvResult;
	}

	return Err;
}

int RtspClient::RecvSDP(int sockfd, string * msg)
{
	MyRegex Regex;
	if(!msg) msg = &SDPStr;

	char * Buf = (char *)calloc(RECV_BUF_SIZE, sizeof(char));
	size_t Size = 0;
	int RecvResult = TRANS_OK;
	string DESCRIBEResponse(RtspResponse);

	msg->assign("");
	string Pattern("Content-Length: +([0-9]+)");
	list<string> Group;
	stringstream tmp;
	if(Regex.Regex(DESCRIBEResponse.c_str(), Pattern.c_str(), &Group)) {
		Group.pop_front();
		tmp << Group.front();
		tmp >> Size;
		if(Size >= RECV_BUF_SIZE) {
			printf("SDP size:Too large\n");
			return TRANS_ERROR;
		}
	} else {
		// Invalid "DESCRIBE" Response
		return TRANS_ERROR;
	}
	RecvResult = RecvSDP(sockfd, Buf, Size);
	if(TRANS_OK == RecvResult) msg->assign(Buf);
	free(Buf);
	return RecvResult;
}
#ifndef _WIN32
int RtspClient::Close(int sockfd)
{
	int CloseResult = -1;
	if(sockfd == RtspSockfd) {
		CloseResult = close(RtspSockfd);
		RtspSockfd = -1;
	} else {
		if(RtspSockfd > 0) {
			CloseResult = close(RtspSockfd);
			RtspSockfd = -1;
		}
		if(sockfd > 0) CloseResult = close(sockfd);
	}
	return CloseResult;
}
#else
int RtspClient::Close(int sockfd)
{
	int CloseResult = -1;
	if (sockfd == RtspSockfd) {
		CloseResult = closesocket(RtspSockfd);
		RtspSockfd = -1;
	}
	else {
		if (RtspSockfd > 0) {
			CloseResult = closesocket(RtspSockfd);
			RtspSockfd = -1;
		}
		if (sockfd > 0) CloseResult = closesocket(sockfd);
	}
	return CloseResult;
}
#endif

string RtspClient::ParseSessionID(string ResponseOfSETUP)
{
	MyRegex Regex;
	string Response("");
	string Result("");

	if(ResponseOfSETUP.length() != 0) Response.assign(ResponseOfSETUP);
	else if(RtspResponse.length() != 0) Response.assign(RtspResponse);
	else return Result;

	// Session: 970756dc30b3a638;timeout=60
	string Pattern("Session: +([0-9a-fA-F_\$-\.\+]+)");
	list<string> Group;
	bool IgnoreCase = true;
	if(Regex.Regex(Response.c_str(), Pattern.c_str(), &Group, IgnoreCase)) {
		Group.pop_front();
		Result.assign(Group.front());
	}
	return Result;
}

int RtspClient::ParseTimeout(string ResponseOfSETUP)
{
	MyRegex Regex;
	string Response("");
	int Result = -1;

	if(ResponseOfSETUP.length() != 0) Response.assign(ResponseOfSETUP);
	else if(RtspResponse.length() != 0) Response.assign(RtspResponse);
	else return Result;

	// Session: 970756dc30b3a638;timeout=60
	string Pattern("Session:.*timeout=([1-9][0-9]*)");
	list<string> Group;
	bool IgnoreCase = true;
	if(Regex.Regex(Response.c_str(), Pattern.c_str(), &Group, IgnoreCase)) {
		Group.pop_front();
		Result =  atoi(Group.front().c_str());
	}
	return Result;
}

bool RtspClient::IsResponse_200_OK(ErrorType * err, string * response)
{
	// example: 
	// "RTSP/1.0 200 OK"

	MyRegex Regex;
	string Pattern200("RTSP/[0-9]+\\.[0-9]+ +200");
	string Pattern401("RTSP/[0-9]+\\.[0-9]+ +401");
	string Pattern40x("RTSP/[0-9]+\\.[0-9]+ +40[0-9]");
	string Pattern50x("RTSP/[0-9]+\\.[0-9]+ +50[0-9]");

	list<string> Groups;
	bool IgnoreCase = true;
	bool Result = false;

	string Response("");

	if(!response) Response.assign(RtspResponse);
	else Response.assign(*response);

	if(Regex.Regex(Response.c_str(), Pattern200.c_str(), &Groups, IgnoreCase)) {
		Result = true;
		if(!err) return Result;
		*err = RTSP_RESPONSE_200;
	} else if(Regex.Regex(Response.c_str(), Pattern401.c_str(), &Groups, IgnoreCase)) {
		Result = false;
		if(!err) return Result;
		*err = RTSP_RESPONSE_401;
	} else if(Regex.Regex(Response.c_str(), Pattern40x.c_str(), &Groups, IgnoreCase)) {
		Result = false;
		if(!err) return Result;
		*err = RTSP_RESPONSE_40X;
	} else if(Regex.Regex(Response.c_str(), Pattern50x.c_str(), &Groups, IgnoreCase)) {
		Result = false;
		if(!err) return Result;
		*err = RTSP_RESPONSE_50X;
	}
	return Result;
}

uint8_t * RtspClient::GetMediaData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size) 
{
	if(!media_session) return NULL;
	return media_session->GetMediaData(buf, size, max_size);
}

uint8_t * RtspClient::GetMediaData(string media_type, uint8_t * buf, size_t * size, size_t max_size) 
{
	MyRegex Regex;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;
	if(!buf) return NULL;
	if(!size) return NULL;

	*size = 0;

    it = MediaSessionMap->find(media_type);
    if(it == MediaSessionMap->end()) {
        printf("regex\n");
        for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
            if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
        }
    }

	if(it == MediaSessionMap->end()) {
		fprintf(stderr, "%s: No such media session\n", __func__);
		return NULL;
	}

	if(it->second.MediaType == "video") return GetVideoData(&(it->second), buf, size, max_size, ObtainVpsSpsPpsPeriodly);
	if(it->second.MediaType == "audio") return GetAudioData(&(it->second), buf, size, max_size);
	return NULL;
}

int RtspClient::GetTimeRate(string media_type)
{
	MyRegex Regex;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it == MediaSessionMap->end()) {
		return -1;
	}
	return it->second.TimeRate;
}

int RtspClient::GetChannelNum(string media_type)
{
	MyRegex Regex;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it == MediaSessionMap->end()) {
		return -1;
	}
	return it->second.ChannelNum;
}

void RtspClient::SetAudioByeFromServerClbk(DESTROIED_CLBK clbk)
{
	ByeFromServerAudioClbk = clbk;
	SetDestroiedClbk("audio", clbk);
}

void RtspClient::SetVideoByeFromServerClbk(DESTROIED_CLBK clbk)
{
	ByeFromServerVideoClbk = clbk;
	SetDestroiedClbk("video", clbk);
}

int RtspClient::GetSessionTimeout(string media_type)
{
	MyRegex Regex;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;
	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}
	if(it == MediaSessionMap->end()) {
		// fprintf(stderr, "%s: No such media session\n", __func__);
		return 0;
	}
	return it->second.Timeout;
}

int RtspClient::GetSessionTimeout(MediaSession * media_session)
{
	if(!media_session) {
		return 0;
	}
	return media_session->Timeout;
}

void RtspClient::SetDestroiedClbk(MediaSession * media_session, DESTROIED_CLBK clbk)
{
	if(media_session) {
		media_session->SetRtpDestroiedClbk(clbk);
	}
}

void RtspClient::SetDestroiedClbk(string media_type, DESTROIED_CLBK clbk)
{
	MyRegex Regex;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;
	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}
	if(it == MediaSessionMap->end()) {
		// fprintf(stderr, "%s: No such media session\n", __func__);
		return;
	}
	it->second.SetRtpDestroiedClbk(clbk);
}

uint8_t * RtspClient::GetVideoData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size, bool get_vps_sps_pps_periodly) 
{
	if(!media_session || !buf || !size) return NULL;

	*size = 0;

	const size_t GetSPS_PPS_Period = GET_SPS_PPS_PERIOD; // 30 times
	if(true == get_vps_sps_pps_periodly) {
		if(GetVideoDataCount >= GetSPS_PPS_Period) {
			GetVideoDataCount = 0;

			const size_t NALU_StartCodeSize = 4;
			size_t SizeTmp = 0;
			if(!GetVPSNalu(buf + (*size), &SizeTmp) || SizeTmp <= NALU_StartCodeSize) {
				// fprintf(stderr, "\033[31mWARNING: No H264 VPS\033[0m\n");
			} else {
				*size += SizeTmp;
			}
			if(!GetSPSNalu(buf + (*size), &SizeTmp) || SizeTmp <= NALU_StartCodeSize) {
				fprintf(stderr, "\033[31mWARNING: No SPS\033[0m\n");
			} else {
				*size += SizeTmp;
			}
			if(!GetPPSNalu(buf + (*size), &SizeTmp) || SizeTmp <= NALU_StartCodeSize) {
				fprintf(stderr, "\033[31mWARNING: No PPS\033[0m\n");
			} else {
				*size += SizeTmp;
			}
			return buf;
		} else {
			GetVideoDataCount++;
		}
	}

	size_t SizeTmp = 0;
	bool EndFlag = false;
	NALUTypeBase * NALUTypeBaseTmp = NULL;
	NALUTypeBase * NALUType;

	int PM = media_session->Packetization;
	if(!IS_PACKET_MODE_VALID(PM)) {
		cerr << "WARNING:Invalid Packetization Mode" << endl;
		return NULL;
	}
	if(media_session->EncodeType == "H264") {
		NALUTypeBaseTmp = &NaluBaseType_H264Obj;
	} else if (media_session->EncodeType == "H265") {
		NALUTypeBaseTmp = &NaluBaseType_H265Obj;
	} else {
		// Unknown Nalu type
		printf("Unsupported codec type: %s\n", media_session->EncodeType.c_str());
		return NULL;
	}

	do {
		EndFlag = true;
		if(!media_session->GetMediaData(VideoBuffer.Buf, &SizeTmp)) return NULL;
		if(0 == SizeTmp) {
			cerr << "No RTP data" << endl;
			return NULL;
		}
		int NT; 
		NT = NALUTypeBaseTmp->ParseNALUHeader_Type(VideoBuffer.Buf);
		NALUType = NALUTypeBaseTmp->GetNaluRtpType(PM, NT);
		//H264�����ҵ����ݰ�ʱ���԰�
		if(NULL == NALUType) {
			printf("Unknown NALU Type: %s\n", media_session->EncodeType.c_str());
			return NULL;
		}

		if(SizeTmp > VideoBuffer.Size) {
			cerr << "Error: RTP Packet too large(" << SizeTmp << " bytes > " << VideoBuffer.Size << "bytes)" << endl;
			return NULL;
		}

		if(*size + SizeTmp > max_size) {
			fprintf(stderr, "\033[31mWARNING: NALU truncated because larger than buffer: %u(NALU size) > %u(Buffer size)\033[0m\n", *size + SizeTmp, max_size);
			return buf;
		}

		SizeTmp = NALUType->CopyData(buf + (*size), VideoBuffer.Buf, SizeTmp);
		*size += SizeTmp;
		EndFlag = NALUType->GetEndFlag();
	} while(!EndFlag);

	return buf;
}

uint8_t * RtspClient::GetAudioData(MediaSession * media_session, uint8_t * buf, size_t * size, size_t max_size)
{
	if(!media_session || !buf || !size) return NULL;

	*size = 0;

	size_t SizeTmp = 0;
	AudioTypeBase * AudioType;

	if(!media_session->GetMediaData(AudioBuffer.Buf, &SizeTmp)) return NULL;
	if(0 == SizeTmp) {
		cerr << "No RTP data" << endl;
		return NULL;
	}


	MyRegex Regex;
    if(Regex.Regex(media_session->EncodeType.c_str(), "PCMU", true)) {
        AudioType = &PCMU_AudioObj;
    } else {
        AudioType = &MPEG_AudioObj;
    }

	if(SizeTmp > AudioBuffer.Size) {
		cerr << "Error: RTP Packet too large(" << SizeTmp << " bytes > " << AudioBuffer.Size << "bytes)" << endl;
		return NULL;
	}

	if(*size + SizeTmp > max_size) {
		fprintf(stderr, "\033[31mWARNING: NALU truncated because larger than buffer: %u(NALU size) > %u(Buffer size)\033[0m\n", *size + SizeTmp, max_size);
		return buf;
	}

	SizeTmp = AudioType->CopyData(buf + (*size), AudioBuffer.Buf, SizeTmp);
	*size += SizeTmp;

	return buf;
}

uint8_t * RtspClient::GetMediaPacket(MediaSession * media_session, uint8_t * buf, size_t * size) 
{
	if(!media_session) return NULL;
	return media_session->GetMediaPacket(buf, size);
}

uint8_t * RtspClient::GetMediaPacket(string media_type, uint8_t * buf, size_t * size) {
	MyRegex Regex;
	map<string, MediaSession>::iterator it;
	bool IgnoreCase = true;

	for(it = MediaSessionMap->begin(); it != MediaSessionMap->end(); it++) {
		if(Regex.Regex(it->first.c_str(), media_type.c_str(), IgnoreCase)) break;
	}

	if(it == MediaSessionMap->end()) {
		fprintf(stderr, "%s: No such media session\n", __func__);
		return NULL;
	}

	return it->second.GetMediaPacket(buf, size);
}

uint8_t * RtspClient::GetVPSNalu(uint8_t * buf, size_t * size)
{
	if(!buf) return NULL;
	if(!size) return NULL;

	*size = 0;

	buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
	*size += 4;

	unsigned int VpsSize = 0;
	unsigned char * Vps = base64Decode(VPS.c_str(), VpsSize, true);
	memcpy(buf + (*size), Vps, VpsSize);
	*size += VpsSize;
	delete[] Vps;
	Vps = NULL;

	return buf;
}

uint8_t * RtspClient::GetSPSNalu(uint8_t * buf, size_t * size)
{
	if(!buf) return NULL;
	if(!size) return NULL;

	*size = 0;

	buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
	*size += 4;

	unsigned int SpsSize = 0;
	unsigned char * Sps = base64Decode(SPS.c_str(), SpsSize, true);
	memcpy(buf + (*size), Sps, SpsSize);
	*size += SpsSize;
	delete[] Sps;
	Sps = NULL;

	return buf;

}

uint8_t * RtspClient::GetPPSNalu(uint8_t * buf, size_t * size)
{
	if(!buf) return NULL;
	if(!size) return NULL;

	*size = 0;

	buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 1;
	*size += 4;

	unsigned int PpsSize = 0;
	unsigned char * Pps = base64Decode(PPS.c_str(), PpsSize, true);
	memcpy(buf + (*size), Pps, PpsSize);
	*size += PpsSize;
	delete[] Pps;
	Pps = NULL;

	return buf;
}

uint32_t RtspClient::CheckAuth(int sockfd, string cmd, string uri)
{
	ErrorType err;
	IsResponse_200_OK(&err);
	// cout << "Start CheckAuth" << endl;
	if(err != RTSP_RESPONSE_401) {
		// cout << "Not 401" << endl;
		return CHECK_OK;
	}
	if(Username.length() == 0) {
		cout << "Username is null" << endl;
		return CHECK_ERROR;
	}
	// cout << "**********" << endl;
	// cout << RtspResponse << endl;
	// cout << "**********" << endl;
	// Response e.g:
	// 	RTSP/1.0 401 Unauthorized
	// 	Server: HiIpcam/V100R003 VodServer/1.0.0
	//	Cseq: 2
	// 	WWW-Authenticate:Digest realm="HipcamRealServer", nonce="3b27a446bfa49b0c48c3edb83139543d"
	MyRegex Regex;
	list<string> Group;
	string PatternDigest("WWW-Authenticate: *Digest +realm=\"([a-zA-Z_0-9 ]+)\", +nonce=\"([a-zA-Z0-9]+)\"");
	string PatternBasic("WWW-Authenticate: *Digest +realm=\"([a-zA-Z_0-9 ]+)\"");
	string RealmTmp("");
	string NonceTmp("");
	string Md5Response("");


	if(Regex.Regex(RtspResponse.c_str(), PatternDigest.c_str(), &Group)) {
		// cout << "Regex hit" << endl;
		Group.pop_front();
		RealmTmp.assign(Group.front());
		Group.pop_front();
		NonceTmp.assign(Group.front());
		Realm.assign(RealmTmp);
		Nonce.assign(NonceTmp);
	} else if(Regex.Regex(RtspResponse.c_str(), PatternBasic.c_str(), &Group)) {
		Group.pop_front();
		RealmTmp.assign(Group.front());
		// Not supported now
		// TODO:
		cout << "BASIC Authentication not supported now" << endl;
		return CHECK_ERROR;
	}
	Md5Response  = MakeMd5DigestResp(RealmTmp, cmd, uri,  NonceTmp);
	if(Md5Response.length() != MD5_SIZE) {
		cout << "Make MD5 digest response error" << endl;
		return CHECK_ERROR;
	}

	stringstream Msg("");
	string RtspUri = RtspURI;
	// cout << "username=" << Username << ";password=" << Password << endl;;
	Msg << cmd << " " << RtspUri << " " << "RTSP/" << VERSION_RTSP << "\r\n";
	Msg << "CSeq: " << ++RtspCSeq << "\r\n";
	Msg << "Authorization: Digest username=\"" << Username << "\", realm=\""
		<< RealmTmp << "\", nonce=\"" << NonceTmp << "\", uri=\"" << uri 
		<< "\", response=\"" << Md5Response << "\"\r\n";
	Msg << "\r\n";
	// cout << Msg.str() << endl;

	if(!SendRTSP(sockfd, Msg.str())) {
		return CHECK_ERROR;
	}
	if(!RecvRTSP(sockfd, &RtspResponse)) {
		return CHECK_ERROR;
	}
	if(!IsResponse_200_OK()) {
		// cout << RtspResponse << endl;
		return CHECK_ERROR;
	}

	return CHECK_OK;
}

string RtspClient::MakeMd5DigestResp(string realm, string cmd, string uri, string nonce, string username, string password)
{
	string tmp("");
	char ha1buf[MD5_BUF_SIZE] = {0};
	char ha2buf[MD5_BUF_SIZE] = {0};
	char habuf[MD5_BUF_SIZE] = {0};

	if(username.length() <= 0) {
		username.assign(Username);
		password.assign(Password);
	}

	tmp.assign("");
	tmp = username + ":" + realm + ":" + password;
	Md5sum32((void *)tmp.c_str(), (unsigned char *)ha1buf, tmp.length(), MD5_BUF_SIZE);
	ha1buf[MD5_SIZE] = '\0';

	tmp.assign("");
	tmp = cmd + ":" + uri;
	Md5sum32((void *)tmp.c_str(), (unsigned char *)ha2buf, tmp.length(), MD5_BUF_SIZE);
	ha2buf[MD5_SIZE] = '\0';
	
	tmp.assign(ha1buf);
	tmp = tmp + ":" + nonce + ":" + ha2buf;
	Md5sum32((void *)tmp.c_str(), (unsigned char *)habuf, tmp.length(), MD5_BUF_SIZE);
	habuf[MD5_SIZE] = '\0';

	tmp.assign("");
	tmp.assign(habuf);

	return tmp;

}

