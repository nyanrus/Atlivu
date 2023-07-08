#pragma once

//--------------------------------------------------------------------

static const int32_t TextMaxSize = 1024;

//--------------------------------------------------------------------

struct MediaInfo {
  int32_t flag;         //	フラグ
  int32_t rate, scale;  //	フレームレート
  int32_t n;            //	フレーム数
  BITMAPINFOHEADER
      format;  //	画像フォーマットへのポインタ(次に関数が呼ばれるまで内容を有効にしておく)
  int32_t format_size;  //	画像フォーマットのサイズ
  int32_t audio_n;      //	音声サンプル数
  WAVEFORMATEX
      audio_format;  //	音声フォーマットへのポインタ(次に関数が呼ばれるまで内容を有効にしておく)
  int32_t audio_format_size;  //	音声フォーマットのサイズ
  DWORD handler;              //	画像codecハンドラ
};

#include <boost/chrono.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
namespace bip = boost::interprocess;

//https://stackoverflow.com/questions/18533527/boostinterprocess-shared-memory-between-32-and-64-bit-processes
using IPC_managed_shared_memory = bip::basic_managed_shared_memory
<
    char, 
    bip::rbtree_best_fit<
        bip::mutex_family,
        bip::offset_ptr<void,int64_t,uint64_t,0>
    >, 
    bip::iset_index
>;


template <typename T>
using ShmemAlloc =
    bip::allocator<T, IPC_managed_shared_memory::segment_manager>;
template <typename T>
using Vec = bip::vector<T, ShmemAlloc<T>>;

//--------------------------------------------------------------------
namespace Input {
//--------------------------------------------------------------------

struct Command {
  enum COM {
    None,
    End,

    /// @param TCHAR filename[TextMaxSize]
    /// @return int32_t result
    LoadPlugin,

    /// @return int32_t result
    UnloadPlugin,

    /// @return int32_t result
    ConfigPlugin,

    /// @param TCHAR fileName[TextMaxSize]
    /// @return int32_t mediaHandle
    OpenMedia,

    /// @param int32_t mediaHandle
    /// @return int32_t result
    CloseMedia,

    /// @param int32_t mediaHandle
    /// @return MediaInfo mediaInfo
    GetMediaInfo,

    /// @param int32_t mediaHandle
    /// @param int32_t frameNum
    /// @return bufferSize
    /// @return BYTE frameBuffer[bufferSize]
    ReadVideo,

    /// @param int32_t mediaHandle
    /// @param int32_t startSampleNum
    /// @param int32_t sampleNum
    /// @return int32_t realSampleNum
    /// @return BYTE audioBuffer[bufferSize] (bufferSize is calcurated with
    /// realSampleNum)
    ReadAudio,
  } com;

  boost::interprocess::interprocess_mutex mutex;

  Command() :  com(COM::None), mutex(bip::interprocess_mutex()) {}
};

//--------------------------------------------------------------------
}  // namespace Input
//--------------------------------------------------------------------

//--------------------------------------------------------------------
namespace Output {
//--------------------------------------------------------------------

struct CommandID {
  static const int32_t None = WM_APP + 100;
  static const int32_t End = WM_APP + 101;
  static const int32_t LoadPlugin = WM_APP + 102;
  static const int32_t UnloadPlugin = WM_APP + 103;
  static const int32_t ConfigPlugin = WM_APP + 104;
  static const int32_t SaveFile = WM_APP + 105;

  static const int32_t IsAbort = WM_APP + 110;
  static const int32_t RestTimeDisp = WM_APP + 111;
  static const int32_t UpdatePreview = WM_APP + 112;
  static const int32_t GetVideo = WM_APP + 113;
  static const int32_t GetAudio = WM_APP + 114;
};

//--------------------------------------------------------------------
}  // namespace Output
//--------------------------------------------------------------------
