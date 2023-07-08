#include "pch.h"
#include "Atlivu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//--------------------------------------------------------------------

BOOL CAtlivuApp::raw_loadInputPlugin(LPCTSTR fileName)
{
	auto& shared_mem = this->m_inputSharedMem;
	
	auto [com,_mm] = shared_mem.find<Input::Command>("g/command");

	{
		// in/out param document is in CommandID source.
		bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);

		com->com = Input::Command::COM::LoadPlugin;
		std::wstring file_name(&fileName[0]);
		auto vec_char = shared_mem.construct<Vec<TCHAR>>("in/LoadPlugin/fileName")(file_name.size(), shared_mem.get_segment_manager());
		std::copy(file_name.begin(), file_name.end(), vec_char->begin());
	}

	
	while (true) {
		{
			bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
			if (com->com == Input::Command::COM::None) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
	
	auto [result,_] = shared_mem.find<int32_t>("out/LoadPlugin/result");
	auto res = *result;
	shared_mem.destroy_ptr(result);

	return res;
}

BOOL CAtlivuApp::raw_unloadInputPlugin()
{
	auto& shared_mem = this->m_inputSharedMem;

	auto [com, _mm] = shared_mem.find<Input::Command>("g/command");

	{
		// in/out param document is in CommandID source.
		bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
		com->com = Input::Command::COM::UnloadPlugin;
	}
	
	while (true) {
		{
			bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
			if (com->com == Input::Command::COM::None) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);

	auto [result, _] = shared_mem.find<int32_t>("out/UnloadPlugin/result");
	auto res = *result;
	shared_mem.destroy_ptr(result);

	return res;
}

//! NOT IMPLEMENTED IN AUI
BOOL CAtlivuApp::raw_configInputPlugin()
{
	auto& shared_mem = this->m_inputSharedMem;

	auto [com, _mm] = shared_mem.find<Input::Command>("g/command");

	{
		// in/out param document is in CommandID source.
		bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
		com->com = Input::Command::COM::ConfigPlugin;
	}

	while (true) {
		{
			bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
			if (com->com == Input::Command::COM::None) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);

	auto [result, _] = shared_mem.find<int32_t>("out/ConfigPlugin/result");
	auto res = *result;
	shared_mem.destroy_ptr(result);

	return res;
}

int32_t CAtlivuApp::raw_openMedia(LPCTSTR fileName)
{
	auto& shared_mem = this->m_inputSharedMem;

	auto [com, _mm] = shared_mem.find<Input::Command>("g/command");

	{
		// in/out param document is in CommandID source.
		bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
		com->com = Input::Command::COM::OpenMedia;

		std::wstring file_name(&fileName[0]);
		auto vec_char = shared_mem.construct<Vec<TCHAR>>("in/OpenMedia/fileName")(file_name.size(), shared_mem.get_segment_manager());
		std::copy(file_name.begin(), file_name.end(), vec_char->begin());
	}

	while (true) {
		{
			bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
			if (com->com == Input::Command::COM::None) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);

	auto [mediaHandle, _] = shared_mem.find<int32_t>("out/OpenMedia/result");
	auto mHandle = *mediaHandle;
	shared_mem.destroy_ptr(mediaHandle);

	return mHandle;
}

BOOL CAtlivuApp::raw_closeMedia(int32_t media)
{
	auto& shared_mem = this->m_inputSharedMem;

	auto [com, _mm] = shared_mem.find<Input::Command>("g/command");

	{
		// in/out param document is in CommandID source.
		bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
		com->com = Input::Command::COM::CloseMedia;

		shared_mem.construct<int32_t>("in/CloseMedia/mediaHandle")(media);
	}

	while (true) {
		{
			bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
			if (com->com == Input::Command::COM::None) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);

	auto [result, _] = shared_mem.find<int32_t>("out/CloseMedia/result");
	auto ret = *result;
	shared_mem.destroy_ptr(result);

	return ret;
}

BOOL CAtlivuApp::raw_getMediaInfo(int32_t media, MediaInfo* mediaInfo)
{
	auto& shared_mem = this->m_inputSharedMem;

	auto [com, _mm] = shared_mem.find<Input::Command>("g/command");

	{
		// in/out param document is in CommandID source.
		bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
		com->com = Input::Command::COM::GetMediaInfo;

		shared_mem.construct<int32_t>("in/GetMediaInfo/mediaHandle")(media);
	}

	while (true) {
		{
			bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
			if (com->com == Input::Command::COM::None) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
	
	auto [med,_] = shared_mem.find<MediaInfo>("out/GetMediaInfo/mediaInfo");

	*mediaInfo = *med;
	shared_mem.destroy_ptr(med);

	return TRUE;
}

BOOL CAtlivuApp::raw_readVideo(int32_t media, int32_t frame, std::vector<BYTE>& output)
{
	CSingleLock lock(&m_csMedia, TRUE);

	auto& shared_mem = this->m_inputSharedMem;

	auto [com, _mm] = shared_mem.find<Input::Command>("g/command");

	{
		// in/out param document is in CommandID source.
		bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
		com->com = Input::Command::COM::ReadVideo;

		shared_mem.construct<int32_t>("in/ReadVideo/mediaHandle")(media);
		shared_mem.construct<int32_t>("in/ReadVideo/frameNum")(frame);

	}

	while (true) {
		{
			bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
			if (com->com == Input::Command::COM::None) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);

	auto [bufSize, _] = shared_mem.find<int32_t>("out/ReadVideo/bufferSize");
	

	if (0 < *bufSize) {
		auto [buf, _] = shared_mem.find<Vec<BYTE>>("out/ReadVideo/buffer");
		output.resize(buf->size());
		std::copy(buf->begin(), buf->end(), output.data());
		shared_mem.destroy_ptr(buf);
	}

	shared_mem.destroy_ptr(bufSize);
	

	return TRUE;
}

BOOL CAtlivuApp::raw_readAudio(int32_t media, int32_t start, int32_t length, int32_t* bufferLength, std::vector<BYTE>& output)
{
	CSingleLock lock(&m_csMedia, TRUE);

	auto& shared_mem = this->m_inputSharedMem;

	auto [com, _mm] = shared_mem.find<Input::Command>("g/command");

	{
		// in/out param document is in CommandID source.
		bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
		com->com = Input::Command::COM::ReadAudio;

		shared_mem.construct<int32_t>("in/ReadAudio/mediaHandle")(media);
		shared_mem.construct<int32_t>("in/ReadVideo/startSampleNum")(start);
		shared_mem.construct<int32_t>("in/ReadVideo/sampleNum")(length);

	}

	while (true) {
		{
			bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);
			if (com->com == Input::Command::COM::None) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	bip::scoped_lock<bip::interprocess_mutex> scoped_lock(com->mutex);

	auto [bufSize, _] = shared_mem.find<int32_t>("out/ReadAudio/realSampleNum");


	if (0 < *bufSize) {
		auto [buf, _] = shared_mem.find<Vec<BYTE>>("out/ReadAudio/audioBuffer");

		*bufferLength = buf->size();

		output.resize(buf->size());
		std::copy(buf->begin(), buf->end(), output.data());
		shared_mem.destroy_ptr(buf);
	}
	shared_mem.destroy_ptr(bufSize);

	return TRUE;
}

//--------------------------------------------------------------------

BOOL CAtlivuApp::raw_loadOutputPlugin(LPCTSTR fileName)
{
	::PostThreadMessage(m_outputPi.dwThreadId, Output::CommandID::LoadPlugin, 0, 0);

	TCHAR fileNameBuffer[TextMaxSize] = {};
	::StringCbCopy(fileNameBuffer, sizeof(fileNameBuffer), fileName);
	writeFile(m_outputPipe, fileNameBuffer, sizeof(fileNameBuffer));

	int32_t result = FALSE;
	readFile(m_outputPipe, &result, sizeof(result));
	return result;
}

BOOL CAtlivuApp::raw_unloadOutputPlugin()
{
	::PostThreadMessage(m_outputPi.dwThreadId, Output::CommandID::UnloadPlugin, 0, 0);

	int32_t result = FALSE;
	readFile(m_outputPipe, &result, sizeof(result));
	return result;
}

BOOL CAtlivuApp::raw_configOutputPlugin()
{
	::PostThreadMessage(m_outputPi.dwThreadId, Output::CommandID::ConfigPlugin, 0, 0);

	int32_t result = FALSE;
	readFile(m_outputPipe, &result, sizeof(result));
	return result;
}

BOOL CAtlivuApp::raw_saveFile(LPCTSTR fileName)
{
	::PostThreadMessage(m_outputPi.dwThreadId, Output::CommandID::SaveFile, 0, 0);

	TCHAR fileNameBuffer[TextMaxSize] = {};
	::StringCbCopy(fileNameBuffer, sizeof(fileNameBuffer), fileName);
	writeFile(m_outputPipe, fileNameBuffer, sizeof(fileNameBuffer));

	writeFile(m_outputPipe, &m_mediaInfo, sizeof(m_mediaInfo));

	return TRUE;
}

//--------------------------------------------------------------------
