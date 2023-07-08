#include "pch.h"
#include "aui.h"
#include "Plugin.h"
#include "Common/Tracer2.h"


//--------------------------------------------------------------------

using namespace Input;

//--------------------------------------------------------------------

HWND g_hostWindow = 0;
//HANDLE g_pipe = 0;
PluginArray g_pluginArray;
MediaMap g_mediaMap;

////--------------------------------------------------------------------
//
//inline DWORD readPipe(void* x, DWORD bufferSize)
//{
//	return readFile(g_pipe, x, bufferSize);
//}
//
//inline DWORD writePipe(const void* x, DWORD bufferSize)
//{
//	return writeFile(g_pipe, x, bufferSize);
//}
//
////--------------------------------------------------------------------
//
void addMedia(MediaPtr media)
{
	g_mediaMap[media.get()] = media;
}

void eraseMedia(Media* mediaPointer)
{
	g_mediaMap.erase(mediaPointer);
}

MediaPtr getMedia(Media* mediaPointer)
{
	auto it = g_mediaMap.find(mediaPointer);
	if (it == g_mediaMap.end())
		return 0;
	else
		return it->second;
}
//
////--------------------------------------------------------------------
//
//void loadPlugin()
//{
//	MY_TRACE(_T("loadPlugin()\n"));
//
//	TCHAR fileName[TextMaxSize] = {};
//	readPipe(fileName, sizeof(fileName));
//	MY_TRACE_TSTR(fileName);
//
//	PluginPtr plugin(new Plugin());
//
//	int32_t result = plugin->load(fileName);
//	if (result) g_pluginArray.push_back(plugin);
//	MY_TRACE_INT(result);
//	writePipe(&result, sizeof(result));
//
//	MY_TRACE(_T("loadPlugin() end\n"));
//}
//
//void unloadPlugin()
//{
//	MY_TRACE(_T("unloadPlugin()\n"));
//
//	for (auto a : g_mediaMap)
//		a.second->close();
//
//	for (auto plugin : g_pluginArray)
//		plugin->unload();
//
//	g_mediaMap.clear();
//	g_pluginArray.clear();
//
//	int32_t result = TRUE;
//	writePipe(&result, sizeof(result));
//
//	MY_TRACE(_T("unloadPlugin() end\n"));
//}
//
//void configPlugin()
//{
//}
//
//void openMedia()
//{
//	MY_TRACE(_T("openMedia()\n"));
//
//	// サーバーからファイル名を読み込む。
//	TCHAR fileName[TextMaxSize] = {};
//	readPipe(fileName, sizeof(fileName));
//
//	for (auto plugin : g_pluginArray)
//	{
//		// メディアを作成する。
//		MediaPtr media(new Media());
//
//		// メディアファイルを開く。
//		if (!media->open(plugin, fileName))
//			continue;
//
//		// メディアマップに追加する。
//		addMedia(media);
//
//		// サーバーに結果 (メディアのポインタ) を書き込む。
//		int32_t result = (int32_t)media.get();
//		writePipe(&result, sizeof(result));
//
//		MY_TRACE(_T("openMedia() succeeded\n"));
//
//		return;
//	}
//
//	// サーバーに結果 (メディアのポインタ) を書き込む。
//	int32_t result = 0;
//	writePipe(&result, sizeof(result));
//
//	MY_TRACE(_T("openMedia() failed\n"));
//}
//
//void closeMedia()
//{
//	MY_TRACE(_T("closeMedia()\n"));
//
//	// サーバーからメディアのポインタを読み込む。
//	Media* mediaPointer = 0;
//	readPipe(&mediaPointer, sizeof(mediaPointer));
//
//	// ポインタから共有ポインタを取得する。
//	MediaPtr media = getMedia(mediaPointer);
//
//	if (!media)
//	{
//		int32_t result = FALSE;
//		writePipe(&result, sizeof(result));
//
//		MY_TRACE(_T("closeMedia() failed\n"));
//
//		return;
//	}
//
//	// メディアファイルを閉じる。
//	if (!media->close())
//	{
//		int32_t result = FALSE;
//		writePipe(&result, sizeof(result));
//
//		MY_TRACE(_T("closeMedia() failed\n"));
//
//		return;
//	}
//
//	// メディアマップから削除する。
//	eraseMedia(mediaPointer);
//
//	int32_t result = TRUE;
//	writePipe(&result, sizeof(result));
//
//	MY_TRACE(_T("closeMedia() succeeded\n"));
//}
//
//void getMediaInfo()
//{
//	MY_TRACE(_T("getMediaInfo()\n"));
//
//	// サーバーからメディアのポインタを読み込む。
//	Media* mediaPointer = 0;
//	readPipe(&mediaPointer, sizeof(mediaPointer));
//
//	// ポインタから共有ポインタを取得する。
//	MediaPtr media = getMedia(mediaPointer);
//
//	if (!media)
//	{
//		MediaInfo result = {};
//		writePipe(&result, sizeof(result));
//
//		MY_TRACE(_T("getMediaInfo() failed\n"));
//
//		return;
//	}
//
//	MediaInfo* mediaInfo = media->getMediaInfo();
//	writePipe(mediaInfo, sizeof(*mediaInfo));
//
//	MY_TRACE(_T("getMediaInfo() succeeded\n"));
//}
//
//void readVideo()
//{
//	MY_TRACE(_T("readVideo()\n"));
//
//	// サーバーからメディアのポインタを読み込む。
//	Media* mediaPointer = 0;
//	readPipe(&mediaPointer, sizeof(mediaPointer));
//	MY_TRACE_HEX(mediaPointer);
//
//	int32_t frame = 0;
//	readPipe(&frame, sizeof(frame));
//	MY_TRACE_INT(frame);
//
//	// ポインタから共有ポインタを取得する。
//	MediaPtr media = getMedia(mediaPointer);
//
//	if (!media)
//	{
//		int32_t bufferSize = 0;
//		writePipe(&bufferSize, sizeof(bufferSize));
//
//		MY_TRACE(_T("readVideo() failed\n"));
//
//		return;
//	}
//
//	int32_t bufferSize = 0;
//	void* buffer = media->readVideo(frame, &bufferSize);
//	writePipe(&bufferSize, sizeof(bufferSize));
//
//	if (bufferSize > 0)
//		writePipe(buffer, bufferSize);
//
//	MY_TRACE(_T("readVideo() succeeded\n"));
//}
//
//void readAudio()
//{
//	MY_TRACE(_T("readAudio()\n"));
//
//	// サーバーからメディアのポインタを読み込む。
//	Media* mediaPointer = 0;
//	readPipe(&mediaPointer, sizeof(mediaPointer));
//	MY_TRACE_HEX(mediaPointer);
//
//	int32_t start = 0;
//	readPipe(&start, sizeof(start));
//	MY_TRACE_INT(start);
//
//	int32_t length = 0;
//	readPipe(&length, sizeof(length));
//	MY_TRACE_INT(length);
//
//	// ポインタから共有ポインタを取得する。
//	MediaPtr media = getMedia(mediaPointer);
//
//	if (!media)
//	{
//		int32_t bufferLength = 0;
//		writePipe(&bufferLength, sizeof(bufferLength));
//
//		MY_TRACE(_T("readAudio() failed\n"));
//
//		return;
//	}
//
//	int32_t bufferLength = 0;
//	void* buffer = media->readAudio(start, length, &bufferLength);
//	MY_TRACE_INT(bufferLength);
//	writePipe(&bufferLength, sizeof(bufferLength));
//	MY_TRACE(_T("bufferLength 書き込み完了\n"));
//
//	if (bufferLength > 0)
//	{
//		int bufferSize = media->calcAudioBufferSize(bufferLength);
//		MY_TRACE_INT(bufferSize);
//		writePipe(buffer, bufferSize);
//		MY_TRACE(_T("buffer 書き込み完了\n"));
//	}
//
//	MY_TRACE(_T("readAudio() succeeded\n"));
//}




namespace bip = boost::interprocess;
template<typename T> using ShmemAlloc = bip::allocator<T, IPC_managed_shared_memory::segment_manager>;
template<typename T> using Vec = bip::vector<T, ShmemAlloc<T>>;

BOOL newMainLoop()
{
	MY_TRACE(_T("newMainLoop()\n"));
	
	IPC_managed_shared_memory shared_mem(bip::open_only, "atlivu_ipc_input");
	
	MY_TRACE(_T("SharedMemOpened\n"));
	//GLOBAL -> use g/ namespace
	Command* com;
	std::tie(com,std::ignore) = shared_mem.find<Command>("g/command");
	MY_TRACE(_T("SharedMemOpened\n"));
	
	MY_TRACE(_T("SharedMemOpened\n"));
	
	while (true) {
		using CommandID = Command::COM;
		//MY_TRACE(boost::posix_time::to_simple_wstring(boost::get_system_time() + boost::posix_time::seconds(1)).c_str()+'\n');
		if (!com->mutex.timed_lock(boost::get_system_time() + boost::posix_time::seconds(1))) {
			if (!::IsWindow(g_hostWindow)) {
				MY_TRACE(_T("ホストウィンドウが無効なのでメインループを終了します\n"));
				return FALSE;
			}
			continue;
		}

		switch (com->com) { 
		case CommandID::End:
			return TRUE;
		case CommandID::LoadPlugin:
		{
			MY_TRACE(_T("NRloadPlugin()\n"));
			//引数読み込み
			Vec<TCHAR>* fileName;
			std::tie(fileName,std::ignore) = shared_mem.find<Vec<TCHAR>>("in/LoadPlugin/fileName");

			PluginPtr plugin(new Plugin());

			std::wstring file_name(fileName->begin(),fileName->end());

			int32_t result = plugin->load(file_name.c_str());

			if (result) g_pluginArray.push_back(plugin);

			MY_TRACE_INT(result);
			//返す
			shared_mem.construct<int32_t>("out/LoadPlugin/result")(result);

			MY_TRACE(_T("NRloadPlugin() end\n"));
			//引数開放
			shared_mem.destroy_ptr(fileName);
			break;
		}
		case CommandID::UnloadPlugin:
		{
			MY_TRACE(_T("NRunloadPlugin()\n"));

			for (auto& a : g_mediaMap)
				a.second->close();

			for (auto& plugin : g_pluginArray)
				plugin->unload();

			g_mediaMap.clear();
			g_pluginArray.clear();

			int32_t result = TRUE;
			shared_mem.construct<int32_t>("out/UnloadPlugin/result")(result);

			MY_TRACE(_T("NRunloadPlugin() end\n"));
			break;
		}
		case CommandID::ConfigPlugin:
		{
			break;
		}
		case CommandID::OpenMedia:
		{
			MY_TRACE(_T("NRopenMedia()\n"));
			Vec<TCHAR>* fileName;
			std::tie(fileName,std::ignore) = shared_mem.find<Vec<TCHAR>>("in/OpenMedia/fileName");
			
			std::wstring file_name(fileName->begin(), fileName->end());

			int32_t result = 0;
			for (auto& plugin : g_pluginArray)
			{
				// メディアを作成する。
				MediaPtr media(new Media());

				// メディアファイルを開く。
				if (!media->open(plugin, file_name.c_str()))
					continue;

				// メディアマップに追加する。
				addMedia(media);

				// サーバーに結果 (メディアのポインタ) を書き込む。
				result = reinterpret_cast<int32_t>(media.get());
			}

			shared_mem.construct<int32_t>("out/OpenMedia/result")(result);
			if (result) 
			{
				MY_TRACE(_T("NRopenMedia() succeeded\n"));
			}
			else 
			{
				MY_TRACE(_T("NRopenMedia() failed\n"));
			}

			shared_mem.destroy_ptr(fileName);
			break;
		}
		case CommandID::CloseMedia:
		{
			MY_TRACE(_T("NRcloseMedia()\n"));

			auto [mediaPointer,_m] = shared_mem.find<int32_t>("in/CloseMedia/mediaPointer");

			// ポインタから共有ポインタを取得する。
			MediaPtr media = getMedia(reinterpret_cast<Media*>(*mediaPointer));

			if (!media)
			{
				int32_t result = FALSE;
				
				shared_mem.construct<int32_t>("out/CloseMedia/result")(result);

				MY_TRACE(_T("NRcloseMedia() failed\n"));

			} else {
				// メディアファイルを閉じる。
				if (!media->close())
				{
					int32_t result = FALSE;
					shared_mem.construct<int32_t>("out/CloseMedia/result")(result);

					MY_TRACE(_T("NRcloseMedia() failed\n"));
				}
				else
				{
					// メディアマップから削除する。
					eraseMedia(reinterpret_cast<Media*>(*mediaPointer));

					int32_t result = TRUE;
					shared_mem.construct<int32_t>("out/CloseMedia/result")(result);

					MY_TRACE(_T("NRcloseMedia() succeeded\n"));
				}
			}
			shared_mem.destroy_ptr(mediaPointer);
			break;
		}
		case CommandID::GetMediaInfo:
		{
			MY_TRACE(_T("NRgetMediaInfo()\n"));
			// サーバーからメディアのポインタを読み込む。
			auto [mediaPointer,_m] = shared_mem.find<int32_t>("in/GetMediaInfo/mediaPointer");

			// ポインタから共有ポインタを取得する。
			MediaPtr media = getMedia(reinterpret_cast<Media*>(*mediaPointer));

			if (!media)
			{
				MediaInfo result = {};
				shared_mem.construct<MediaInfo>("out/GetMediaInfo/mediaInfo")(result);

				MY_TRACE(_T("NRgetMediaInfo() failed\n"));
			} else {
				MediaInfo* mediaInfo = media->getMediaInfo();
				shared_mem.construct<MediaInfo>("out/GetMediaInfo/mediaInfo")(*mediaInfo);

				MY_TRACE(_T("NRgetMediaInfo() succeeded\n"));
			}	
			shared_mem.destroy_ptr(mediaPointer);
			break;
		}
		case CommandID::ReadVideo:
		{
			MY_TRACE(_T("readVideo()\n"));

			// サーバーからメディアのポインタを読み込む。
			auto [mediaPointer,_m] = shared_mem.find<int32_t>("in/ReadVideo/mediaPointer");
			MY_TRACE_HEX(reinterpret_cast<Media*>(*mediaPointer));

			auto [frame,_f] = shared_mem.find<int32_t>("in/ReadVideo/frame");
			MY_TRACE_INT(frame);

			// ポインタから共有ポインタを取得する。
			MediaPtr media = getMedia(reinterpret_cast<Media*>(*mediaPointer));

			if (!media)
			{
				int32_t bufferSize = 0;
				shared_mem.construct<int32_t>("out/ReadVideo/bufferSize")(bufferSize);

				MY_TRACE(_T("NRreadVideo() failed\n"));
			}
			else {
				int32_t bufferSize = 0;
				auto buffer = reinterpret_cast<BYTE*>(media->readVideo(*frame, &bufferSize));
				shared_mem.construct<int32_t>("out/ReadVideo/bufferSize")(bufferSize);

				if (0 < bufferSize) {
					const ShmemAlloc<Vec<BYTE>> alloc_inst(shared_mem.get_segment_manager());
					auto vec = shared_mem.construct<Vec<BYTE>>("out/ReadVideo/buffer")(alloc_inst);
					
					std::copy(buffer, buffer + bufferSize, vec->data());
				}
				MY_TRACE(_T("NRreadVideo() succeeded\n"));
			}
			shared_mem.destroy_ptr(mediaPointer);
			shared_mem.destroy_ptr(frame);
			break;
		}
		case CommandID::ReadAudio: {
			MY_TRACE(_T("NRreadAudio()\n"));

			// サーバーからメディアのポインタを読み込む。
			auto [mediaPointer,_m] = shared_mem.find<int32_t>("in/ReadAudio/mediaPointer");
			MY_TRACE_HEX(reinterpret_cast<Media*>(*mediaPointer));

			auto [start, _s] = shared_mem.find<int32_t>("in/ReadAudio/start");
			MY_TRACE_INT(*start);

			auto [length, _l] = shared_mem.find<int32_t>("in/ReadAudio/length");
			MY_TRACE_INT(*length);

			// ポインタから共有ポインタを取得する。
			MediaPtr media = getMedia(reinterpret_cast<Media*>(mediaPointer));

			if (!media)
			{
				int32_t bufferLength = 0;
				shared_mem.construct<int32_t>("out/ReadAudio/bufferLength")(bufferLength);

				MY_TRACE(_T("NRreadAudio() failed\n"));
			}
			else {
				int32_t bufferLength = 0;
				auto buffer = reinterpret_cast<BYTE*>(media->readAudio(*start, *length, &bufferLength));
				MY_TRACE_INT(bufferLength);
				shared_mem.construct<int32_t>("out/ReadAudio/bufferLength")(bufferLength);
				MY_TRACE(_T("bufferLength 書き込み完了\n"));

				if (0 < bufferLength)
				{
					int bufferSize = media->calcAudioBufferSize(bufferLength);
					const ShmemAlloc<Vec<BYTE>> alloc_inst(shared_mem.get_segment_manager());
					auto* vec = shared_mem.construct<Vec<BYTE>>("out/ReadAudio/buffer")(alloc_inst);
					std::copy(buffer, buffer + bufferSize, vec->data());
					MY_TRACE(_T("buffer 書き込み完了\n"));
				}

				MY_TRACE(_T("NRreadAudio() succeeded\n"));
			}
			shared_mem.destroy_ptr(mediaPointer);
			shared_mem.destroy_ptr(start);
			shared_mem.destroy_ptr(length);
			break;
		}
		default: {}
		}
		com->com = Command::COM::None;
		com->mutex.unlock();
	}
}

//BOOL mainLoop()
//{
//	MY_TRACE(_T("mainLoop()\n"));
//
//	UINT_PTR timerId = ::SetTimer(0, 0, 1000, 0);
//
//	MSG msg = {};
//	while (::GetMessage(&msg, 0, 0, 0))
//	{
//		switch (msg.message)
//		{
//		case WM_TIMER:
//			{
//				if (!msg.hwnd && msg.wParam == timerId)
//				{
//					if (!::IsWindow(g_hostWindow))
//					{
//						MY_TRACE(_T("ホストウィンドウが無効なのでメインループを終了します\n"));
//
//						return FALSE;
//					}
//				}
//
//				break;
//			}
//		case WM_CLOSE:
//			{
//				if (!msg.hwnd)
//				{
//					MY_TRACE(_T("WM_CLOSE が投げられたのでメインループを終了します\n"));
//
//					return FALSE;
//				}
//
//				break;
//			}
//		case CommandID::End:
//			{
//				return TRUE;
//			}
//		case CommandID::LoadPlugin:
//			{
//				loadPlugin();
//
//				break;
//			}
//		case CommandID::UnloadPlugin:
//			{
//				unloadPlugin();
//
//				break;
//			}
//		case CommandID::ConfigPlugin:
//			{
//				configPlugin();
//
//				break;
//			}
//		case CommandID::OpenMedia:
//			{
//				openMedia();
//
//				break;
//			}
//		case CommandID::CloseMedia:
//			{
//				closeMedia();
//
//				break;
//			}
//		case CommandID::GetMediaInfo:
//			{
//				getMediaInfo();
//
//				break;
//			}
//		case CommandID::ReadVideo:
//			{
//				readVideo();
//
//				break;
//			}
//		case CommandID::ReadAudio:
//			{
//				readAudio();
//
//				break;
//			}
//		}
//
//		::TranslateMessage(&msg);
//		::DispatchMessage(&msg);
//	}
//
//	return TRUE;
//}

BOOL onWinMain(LPTSTR cmdLine)
{
	MY_TRACE(_T("onWinMain(%s)\n"), cmdLine);

	LPTSTR commandLine = ::GetCommandLine();
	MY_TRACE_TSTR(commandLine);
	g_hostWindow = (HWND)_tcstoul(commandLine, 0, 0);
	MY_TRACE_HWND(g_hostWindow);

	DWORD pid = 0;
	DWORD tid = ::GetWindowThreadProcessId(g_hostWindow, &pid);
	MY_TRACE_INT(pid);
	MY_TRACE_INT(tid);

	/*TCHAR pipeName[MAX_PATH] = {};
	::StringCbPrintf(pipeName, sizeof(pipeName), _T("\\\\.\\pipe\\aui%d"), tid);
	MY_TRACE_TSTR(pipeName);

	g_pipe = ::CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	MY_TRACE_HEX(g_pipe);

	DWORD dwMode = PIPE_READMODE_BYTE;
	BOOL fSuccess = ::SetNamedPipeHandleState(g_pipe, &dwMode, NULL, NULL);
	MY_TRACE_HEX(dwMode);
	MY_TRACE_INT(fSuccess);*/

	//mainLoop();
	newMainLoop();

	//::CloseHandle(g_pipe);

	return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPTSTR cmdLine, int cmdShow)
{
	::CoInitialize(0);

	_tsetlocale(LC_CTYPE, _T(""));

	trace_init(0, 0, TRUE);

	onWinMain(cmdLine);

	trace_term();

	::CoUninitialize();

	return 0;
}

//--------------------------------------------------------------------
