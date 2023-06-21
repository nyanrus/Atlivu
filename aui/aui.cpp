#include "pch.h"
#include "aui.h"
#include "Plugin.h"
#include "Common/Tracer2.h"
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time.hpp>

//--------------------------------------------------------------------

using namespace Input;

//--------------------------------------------------------------------

HWND g_hostWindow = 0;
HANDLE g_pipe = 0;
PluginArray g_pluginArray;
MediaMap g_mediaMap;

//--------------------------------------------------------------------

inline DWORD readPipe(void* x, DWORD bufferSize)
{
	return readFile(g_pipe, x, bufferSize);
}

inline DWORD writePipe(const void* x, DWORD bufferSize)
{
	return writeFile(g_pipe, x, bufferSize);
}

//--------------------------------------------------------------------

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

//--------------------------------------------------------------------

void loadPlugin()
{
	MY_TRACE(_T("loadPlugin()\n"));

	TCHAR fileName[TextMaxSize] = {};
	readPipe(fileName, sizeof(fileName));
	MY_TRACE_TSTR(fileName);

	PluginPtr plugin(new Plugin());

	int32_t result = plugin->load(fileName);
	if (result) g_pluginArray.push_back(plugin);
	MY_TRACE_INT(result);
	writePipe(&result, sizeof(result));

	MY_TRACE(_T("loadPlugin() end\n"));
}

void unloadPlugin()
{
	MY_TRACE(_T("unloadPlugin()\n"));

	for (auto a : g_mediaMap)
		a.second->close();

	for (auto plugin : g_pluginArray)
		plugin->unload();

	g_mediaMap.clear();
	g_pluginArray.clear();

	int32_t result = TRUE;
	writePipe(&result, sizeof(result));

	MY_TRACE(_T("unloadPlugin() end\n"));
}

void configPlugin()
{
}

void openMedia()
{
	MY_TRACE(_T("openMedia()\n"));

	// サーバーからファイル名を読み込む。
	TCHAR fileName[TextMaxSize] = {};
	readPipe(fileName, sizeof(fileName));

	for (auto plugin : g_pluginArray)
	{
		// メディアを作成する。
		MediaPtr media(new Media());

		// メディアファイルを開く。
		if (!media->open(plugin, fileName))
			continue;

		// メディアマップに追加する。
		addMedia(media);

		// サーバーに結果 (メディアのポインタ) を書き込む。
		int32_t result = (int32_t)media.get();
		writePipe(&result, sizeof(result));

		MY_TRACE(_T("openMedia() succeeded\n"));

		return;
	}

	// サーバーに結果 (メディアのポインタ) を書き込む。
	int32_t result = 0;
	writePipe(&result, sizeof(result));

	MY_TRACE(_T("openMedia() failed\n"));
}

void closeMedia()
{
	MY_TRACE(_T("closeMedia()\n"));

	// サーバーからメディアのポインタを読み込む。
	Media* mediaPointer = 0;
	readPipe(&mediaPointer, sizeof(mediaPointer));

	// ポインタから共有ポインタを取得する。
	MediaPtr media = getMedia(mediaPointer);

	if (!media)
	{
		int32_t result = FALSE;
		writePipe(&result, sizeof(result));

		MY_TRACE(_T("closeMedia() failed\n"));

		return;
	}

	// メディアファイルを閉じる。
	if (!media->close())
	{
		int32_t result = FALSE;
		writePipe(&result, sizeof(result));

		MY_TRACE(_T("closeMedia() failed\n"));

		return;
	}

	// メディアマップから削除する。
	eraseMedia(mediaPointer);

	int32_t result = TRUE;
	writePipe(&result, sizeof(result));

	MY_TRACE(_T("closeMedia() succeeded\n"));
}

void getMediaInfo()
{
	MY_TRACE(_T("getMediaInfo()\n"));

	// サーバーからメディアのポインタを読み込む。
	Media* mediaPointer = 0;
	readPipe(&mediaPointer, sizeof(mediaPointer));

	// ポインタから共有ポインタを取得する。
	MediaPtr media = getMedia(mediaPointer);

	if (!media)
	{
		MediaInfo result = {};
		writePipe(&result, sizeof(result));

		MY_TRACE(_T("getMediaInfo() failed\n"));

		return;
	}

	MediaInfo* mediaInfo = media->getMediaInfo();
	writePipe(mediaInfo, sizeof(*mediaInfo));

	MY_TRACE(_T("getMediaInfo() succeeded\n"));
}

void readVideo()
{
	MY_TRACE(_T("readVideo()\n"));

	// サーバーからメディアのポインタを読み込む。
	Media* mediaPointer = 0;
	readPipe(&mediaPointer, sizeof(mediaPointer));
	MY_TRACE_HEX(mediaPointer);

	int32_t frame = 0;
	readPipe(&frame, sizeof(frame));
	MY_TRACE_INT(frame);

	// ポインタから共有ポインタを取得する。
	MediaPtr media = getMedia(mediaPointer);

	if (!media)
	{
		int32_t bufferSize = 0;
		writePipe(&bufferSize, sizeof(bufferSize));

		MY_TRACE(_T("readVideo() failed\n"));

		return;
	}

	int32_t bufferSize = 0;
	void* buffer = media->readVideo(frame, &bufferSize);
	writePipe(&bufferSize, sizeof(bufferSize));

	if (bufferSize > 0)
		writePipe(buffer, bufferSize);

	MY_TRACE(_T("readVideo() succeeded\n"));
}

void readAudio()
{
	MY_TRACE(_T("readAudio()\n"));

	// サーバーからメディアのポインタを読み込む。
	Media* mediaPointer = 0;
	readPipe(&mediaPointer, sizeof(mediaPointer));
	MY_TRACE_HEX(mediaPointer);

	int32_t start = 0;
	readPipe(&start, sizeof(start));
	MY_TRACE_INT(start);

	int32_t length = 0;
	readPipe(&length, sizeof(length));
	MY_TRACE_INT(length);

	// ポインタから共有ポインタを取得する。
	MediaPtr media = getMedia(mediaPointer);

	if (!media)
	{
		int32_t bufferLength = 0;
		writePipe(&bufferLength, sizeof(bufferLength));

		MY_TRACE(_T("readAudio() failed\n"));

		return;
	}

	int32_t bufferLength = 0;
	void* buffer = media->readAudio(start, length, &bufferLength);
	MY_TRACE_INT(bufferLength);
	writePipe(&bufferLength, sizeof(bufferLength));
	MY_TRACE(_T("bufferLength 書き込み完了\n"));

	if (bufferLength > 0)
	{
		int bufferSize = media->calcAudioBufferSize(bufferLength);
		MY_TRACE_INT(bufferSize);
		writePipe(buffer, bufferSize);
		MY_TRACE(_T("buffer 書き込み完了\n"));
	}

	MY_TRACE(_T("readAudio() succeeded\n"));
}

BOOL newMainLoop()
{
	MY_TRACE(_T("newMainLoop()\n"));
	using namespace boost::interprocess;
	boost::interprocess::named_semaphore sp(open_only, "semaphore");
	const void* ptr = nullptr;
	boost::interprocess::managed_shared_memory shared_mem(open_only, "Name", ptr);
	auto mutex = shared_mem.find<boost::interprocess::interprocess_mutex>("g/mutex");
	auto com = shared_mem.find<int32_t>("g/command");
	
	while (true) {
		if (!mutex.first->timed_lock(boost::posix_time::seconds(1))) {
			if (!::IsWindow(g_hostWindow)) {
				MY_TRACE(_T("ホストウィンドウが無効なのでメインループを終了します\n"));
				return FALSE;
			}
			continue;
		}

		if (shared_mem.get_free_memory() <= 1024) {
			shared_mem.grow("Name", 1024);
		}

		switch (*com.first) { 
		case CommandID::End:
			return TRUE;
		case CommandID::LoadPlugin:
		{
			MY_TRACE(_T("NRloadPlugin()\n"));
			//引数読み込み
			auto fileName = shared_mem.find<TCHAR>("in/LoadPlugin/fileName");

			PluginPtr plugin(new Plugin());

			int32_t result = plugin->load(fileName.first);
			if (result) g_pluginArray.push_back(plugin);
			MY_TRACE_INT(result);
			//返す
			shared_mem.construct<int32_t>("out/LoadPlugin/result")(result);

			MY_TRACE(_T("NRloadPlugin() end\n"));
			//引数開放
			shared_mem.destroy_ptr(fileName.first);
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
			auto fileName = shared_mem.find<TCHAR>("in/OpenMedia/fileName");
			int32_t result = 0;

			for (auto& plugin : g_pluginArray)
			{
				// メディアを作成する。
				MediaPtr media(new Media());

				// メディアファイルを開く。
				if (!media->open(plugin, fileName.first))
					continue;

				// メディアマップに追加する。
				addMedia(media);

				// サーバーに結果 (メディアのポインタ) を書き込む。
				result = (int32_t)media.get();
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
			shared_mem.destroy_ptr(fileName.first);
			break;
		}
		case CommandID::CloseMedia:
		{
			MY_TRACE(_T("NRcloseMedia()\n"));

			auto mediaPointer = shared_mem.find<int32_t>("in/CloseMedia/mediaPointer");

			// ポインタから共有ポインタを取得する。
			MediaPtr media = getMedia(reinterpret_cast<Media*>(*mediaPointer.first));

			if (!media)
			{
				int32_t result = FALSE;
				
				shared_mem.construct<int32_t>("out/CloseMedia/result")(result);
				shared_mem.destroy_ptr(mediaPointer.first);

				MY_TRACE(_T("NRcloseMedia() failed\n"));

				break;
			}

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
				eraseMedia(reinterpret_cast<Media*>(*mediaPointer.first));

				int32_t result = TRUE;
				shared_mem.construct<int32_t>("out/CloseMedia/result")(result);

				MY_TRACE(_T("NRcloseMedia() succeeded\n"));
			}
			shared_mem.destroy_ptr(mediaPointer.first);
			break;
		}
		case CommandID::GetMediaInfo:
		{
			MY_TRACE(_T("NRgetMediaInfo()\n"));

			// サーバーからメディアのポインタを読み込む。
			auto mediaPointer = shared_mem.find<int32_t>("in/GetMediaInfo/mediaPointer");

			// ポインタから共有ポインタを取得する。
			MediaPtr media = getMedia(reinterpret_cast<Media*>(*mediaPointer.first));

			if (!media)
			{
				MediaInfo result = {};
				shared_mem.construct<MediaInfo>("out/GetMediaInfo/mediaInfo")(result);

				MY_TRACE(_T("NRgetMediaInfo() failed\n"));
			} else {
				MediaInfo* mediaInfo = media->getMediaInfo();
				shared_mem.construct<MediaInfo>("out/GetMediaInfo/mediaInfo")(mediaInfo);

				MY_TRACE(_T("NRgetMediaInfo() succeeded\n"));
			}	
			shared_mem.destroy_ptr(mediaPointer.first);
		}
		case CommandID::ReadVideo:
		{
			MY_TRACE(_T("readVideo()\n"));

			// サーバーからメディアのポインタを読み込む。
			auto mediaPointer = shared_mem.find<int32_t>("in/ReadVideo/mediaPointer");
			MY_TRACE_HEX(reinterpret_cast<Media*>(*mediaPointer.first));

			auto frame = shared_mem.find<int32_t>("in/ReadVideo/frame");
			MY_TRACE_INT(frame);

			// ポインタから共有ポインタを取得する。
			MediaPtr media = getMedia(reinterpret_cast<Media*>(*mediaPointer.first));

			if (!media)
			{
				int32_t bufferSize = 0;
				shared_mem.construct<int32_t>("out/ReadVideo/bufferSize")(bufferSize);

				MY_TRACE(_T("NRreadVideo() failed\n"));
			}
			else {
				int32_t bufferSize = 0;
				void* buffer = media->readVideo(*frame.first, &bufferSize);
				shared_mem.construct<int32_t>("out/ReadVideo/bufferSize")(bufferSize);

				if (bufferSize > 0) {
					shared_mem.grow("Name",(ceilf(static_cast<float>(bufferSize - shared_mem.get_free_memory()) / 1024.f) + 1));
					shared_mem.construct<BYTE>("out/ReadVideo/buffer")[bufferSize](buffer); 
				}

				MY_TRACE(_T("NRreadVideo() succeeded\n"));
			}
			shared_mem.destroy_ptr(mediaPointer.first);
			shared_mem.destroy_ptr(frame.first);
		}
		
		}
		*com.first = -1;
		mutex.first->unlock();
	}
}

BOOL mainLoop()
{
	MY_TRACE(_T("mainLoop()\n"));

	UINT_PTR timerId = ::SetTimer(0, 0, 1000, 0);

	MSG msg = {};
	while (::GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
		case WM_TIMER:
			{
				if (!msg.hwnd && msg.wParam == timerId)
				{
					if (!::IsWindow(g_hostWindow))
					{
						MY_TRACE(_T("ホストウィンドウが無効なのでメインループを終了します\n"));

						return FALSE;
					}
				}

				break;
			}
		case WM_CLOSE:
			{
				if (!msg.hwnd)
				{
					MY_TRACE(_T("WM_CLOSE が投げられたのでメインループを終了します\n"));

					return FALSE;
				}

				break;
			}
		case CommandID::End:
			{
				return TRUE;
			}
		case CommandID::LoadPlugin:
			{
				loadPlugin();

				break;
			}
		case CommandID::UnloadPlugin:
			{
				unloadPlugin();

				break;
			}
		case CommandID::ConfigPlugin:
			{
				configPlugin();

				break;
			}
		case CommandID::OpenMedia:
			{
				openMedia();

				break;
			}
		case CommandID::CloseMedia:
			{
				closeMedia();

				break;
			}
		case CommandID::GetMediaInfo:
			{
				getMediaInfo();

				break;
			}
		case CommandID::ReadVideo:
			{
				readVideo();

				break;
			}
		case CommandID::ReadAudio:
			{
				readAudio();

				break;
			}
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return TRUE;
}

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

	TCHAR pipeName[MAX_PATH] = {};
	::StringCbPrintf(pipeName, sizeof(pipeName), _T("\\\\.\\pipe\\aui%d"), tid);
	MY_TRACE_TSTR(pipeName);

	g_pipe = ::CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	MY_TRACE_HEX(g_pipe);

	DWORD dwMode = PIPE_READMODE_BYTE;
	BOOL fSuccess = ::SetNamedPipeHandleState(g_pipe, &dwMode, NULL, NULL);
	MY_TRACE_HEX(dwMode);
	MY_TRACE_INT(fSuccess);

	mainLoop();

	::CloseHandle(g_pipe);

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
