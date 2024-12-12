// ignore_for_file: always_specify_types
// ignore_for_file: camel_case_types
// ignore_for_file: non_constant_identifier_names

// AUTO GENERATED FILE, DO NOT EDIT.
//
// Generated by `package:ffigen`.
// ignore_for_file: type=lint
import 'dart:ffi' as ffi;

/// Bindings for `src/pro_miniaudio.h`.
///
/// Regenerate bindings with `dart run ffigen --config ffigen.yaml`.
///
class ProMiniaudioBindings {
  /// Holds the symbol lookup function.
  final ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
      _lookup;

  /// The symbols are looked up in [dynamicLibrary].
  ProMiniaudioBindings(ffi.DynamicLibrary dynamicLibrary)
      : _lookup = dynamicLibrary.lookup;

  /// The symbols are looked up with [lookup].
  ProMiniaudioBindings.fromLookup(
      ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
          lookup)
      : _lookup = lookup;

  /// Creates a new audio context.
  ffi.Pointer<ffi.Void> audio_context_create() {
    return _audio_context_create();
  }

  late final _audio_context_createPtr =
      _lookup<ffi.NativeFunction<ffi.Pointer<ffi.Void> Function()>>(
          'audio_context_create');
  late final _audio_context_create =
      _audio_context_createPtr.asFunction<ffi.Pointer<ffi.Void> Function()>();

  /// Destroys an audio context.
  void audio_context_destroy(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _audio_context_destroy(
      self,
    );
  }

  late final _audio_context_destroyPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>(
          'audio_context_destroy');
  late final _audio_context_destroy = _audio_context_destroyPtr
      .asFunction<void Function(ffi.Pointer<ffi.Void>)>();

  /// Refreshes the list of available audio devices.
  void audio_context_refresh_devices(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _audio_context_refresh_devices(
      self,
    );
  }

  late final _audio_context_refresh_devicesPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>(
          'audio_context_refresh_devices');
  late final _audio_context_refresh_devices = _audio_context_refresh_devicesPtr
      .asFunction<void Function(ffi.Pointer<ffi.Void>)>();

  /// Retrieves the number of playback devices.
  int audio_context_get_playback_device_count(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _audio_context_get_playback_device_count(
      self,
    );
  }

  late final _audio_context_get_playback_device_countPtr =
      _lookup<ffi.NativeFunction<ffi.Uint32 Function(ffi.Pointer<ffi.Void>)>>(
          'audio_context_get_playback_device_count');
  late final _audio_context_get_playback_device_count =
      _audio_context_get_playback_device_countPtr
          .asFunction<int Function(ffi.Pointer<ffi.Void>)>();

  /// Retrieves the number of capture devices.
  int audio_context_get_capture_device_count(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _audio_context_get_capture_device_count(
      self,
    );
  }

  late final _audio_context_get_capture_device_countPtr =
      _lookup<ffi.NativeFunction<ffi.Uint32 Function(ffi.Pointer<ffi.Void>)>>(
          'audio_context_get_capture_device_count');
  late final _audio_context_get_capture_device_count =
      _audio_context_get_capture_device_countPtr
          .asFunction<int Function(ffi.Pointer<ffi.Void>)>();

  /// Gets detailed information about playback devices.
  ffi.Pointer<device_info_t> audio_context_get_playback_device_infos(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _audio_context_get_playback_device_infos(
      self,
    );
  }

  late final _audio_context_get_playback_device_infosPtr = _lookup<
          ffi.NativeFunction<
              ffi.Pointer<device_info_t> Function(ffi.Pointer<ffi.Void>)>>(
      'audio_context_get_playback_device_infos');
  late final _audio_context_get_playback_device_infos =
      _audio_context_get_playback_device_infosPtr.asFunction<
          ffi.Pointer<device_info_t> Function(ffi.Pointer<ffi.Void>)>();

  /// Gets detailed information about capture devices.
  ffi.Pointer<device_info_t> audio_context_get_capture_device_infos(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _audio_context_get_capture_device_infos(
      self,
    );
  }

  late final _audio_context_get_capture_device_infosPtr = _lookup<
          ffi.NativeFunction<
              ffi.Pointer<device_info_t> Function(ffi.Pointer<ffi.Void>)>>(
      'audio_context_get_capture_device_infos');
  late final _audio_context_get_capture_device_infos =
      _audio_context_get_capture_device_infosPtr.asFunction<
          ffi.Pointer<device_info_t> Function(ffi.Pointer<ffi.Void>)>();

  /// Sets the current log level.
  void set_log_level(
    LogLevel level,
  ) {
    return _set_log_level(
      level.value,
    );
  }

  late final _set_log_levelPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.UnsignedInt)>>(
          'set_log_level');
  late final _set_log_level =
      _set_log_levelPtr.asFunction<void Function(int)>();

  /// Enables or disables logging to a file.
  void set_log_to_file_enabled(
    bool enabled,
  ) {
    return _set_log_to_file_enabled(
      enabled,
    );
  }

  late final _set_log_to_file_enabledPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Bool)>>(
          'set_log_to_file_enabled');
  late final _set_log_to_file_enabled =
      _set_log_to_file_enabledPtr.asFunction<void Function(bool)>();

  /// Checks if file logging is currently enabled.
  bool is_log_to_file_enabled() {
    return _is_log_to_file_enabled();
  }

  late final _is_log_to_file_enabledPtr =
      _lookup<ffi.NativeFunction<ffi.Bool Function()>>(
          'is_log_to_file_enabled');
  late final _is_log_to_file_enabled =
      _is_log_to_file_enabledPtr.asFunction<bool Function()>();

  /// Initializes logging to a file.
  void init_file_log(
    ffi.Pointer<ffi.Char> filename,
  ) {
    return _init_file_log(
      filename,
    );
  }

  late final _init_file_logPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Char>)>>(
          'init_file_log');
  late final _init_file_log =
      _init_file_logPtr.asFunction<void Function(ffi.Pointer<ffi.Char>)>();

  /// Closes the currently opened log file.
  void close_file_log() {
    return _close_file_log();
  }

  late final _close_file_logPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function()>>('close_file_log');
  late final _close_file_log = _close_file_logPtr.asFunction<void Function()>();

  /// Enables or disables logging to the console.
  void set_log_to_console_enabled(
    bool enabled,
  ) {
    return _set_log_to_console_enabled(
      enabled,
    );
  }

  late final _set_log_to_console_enabledPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Bool)>>(
          'set_log_to_console_enabled');
  late final _set_log_to_console_enabled =
      _set_log_to_console_enabledPtr.asFunction<void Function(bool)>();

  /// Logs a message with the specified severity level.
  void log_message(
    LogLevel level,
    ffi.Pointer<ffi.Char> funcName,
    ffi.Pointer<ffi.Char> format,
  ) {
    return _log_message(
      level.value,
      funcName,
      format,
    );
  }

  late final _log_messagePtr = _lookup<
      ffi.NativeFunction<
          ffi.Void Function(ffi.UnsignedInt, ffi.Pointer<ffi.Char>,
              ffi.Pointer<ffi.Char>)>>('log_message');
  late final _log_message = _log_messagePtr.asFunction<
      void Function(int, ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Char>)>();

  /// Creates a playback device with the specified parameters.
  ffi.Pointer<ffi.Void> playback_device_create(
    ffi.Pointer<ffi.Void> pContext,
    ffi.Pointer<ffi.Void> pDeviceId,
    playback_config_t config,
  ) {
    return _playback_device_create(
      pContext,
      pDeviceId,
      config,
    );
  }

  late final _playback_device_createPtr = _lookup<
      ffi.NativeFunction<
          ffi.Pointer<ffi.Void> Function(
              ffi.Pointer<ffi.Void>,
              ffi.Pointer<ffi.Void>,
              playback_config_t)>>('playback_device_create');
  late final _playback_device_create = _playback_device_createPtr.asFunction<
      ffi.Pointer<ffi.Void> Function(
          ffi.Pointer<ffi.Void>, ffi.Pointer<ffi.Void>, playback_config_t)>();

  /// Destroys a playback device and releases its resources.
  void playback_device_destroy(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _playback_device_destroy(
      self,
    );
  }

  late final _playback_device_destroyPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>(
          'playback_device_destroy');
  late final _playback_device_destroy = _playback_device_destroyPtr
      .asFunction<void Function(ffi.Pointer<ffi.Void>)>();

  /// Resets the playback device's internal buffer.
  void playback_device_reset_buffer(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _playback_device_reset_buffer(
      self,
    );
  }

  late final _playback_device_reset_bufferPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>(
          'playback_device_reset_buffer');
  late final _playback_device_reset_buffer = _playback_device_reset_bufferPtr
      .asFunction<void Function(ffi.Pointer<ffi.Void>)>();

  /// Starts audio playback on the specified device.
  void playback_device_start(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _playback_device_start(
      self,
    );
  }

  late final _playback_device_startPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>(
          'playback_device_start');
  late final _playback_device_start = _playback_device_startPtr
      .asFunction<void Function(ffi.Pointer<ffi.Void>)>();

  /// Stops audio playback on the specified device.
  void playback_device_stop(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _playback_device_stop(
      self,
    );
  }

  late final _playback_device_stopPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>(
          'playback_device_stop');
  late final _playback_device_stop = _playback_device_stopPtr
      .asFunction<void Function(ffi.Pointer<ffi.Void>)>();

  /// Pushes audio data into the playback device's buffer.
  void playback_device_push_buffer(
    ffi.Pointer<ffi.Void> self,
    ffi.Pointer<playback_data_t> pData,
  ) {
    return _playback_device_push_buffer(
      self,
      pData,
    );
  }

  late final _playback_device_push_bufferPtr = _lookup<
      ffi.NativeFunction<
          ffi.Void Function(ffi.Pointer<ffi.Void>,
              ffi.Pointer<playback_data_t>)>>('playback_device_push_buffer');
  late final _playback_device_push_buffer =
      _playback_device_push_bufferPtr.asFunction<
          void Function(ffi.Pointer<ffi.Void>, ffi.Pointer<playback_data_t>)>();

  /// Retrieves the current state of the playback device.
  device_state_t playback_device_get_state(
    ffi.Pointer<ffi.Void> self,
  ) {
    return device_state_t.fromValue(_playback_device_get_state(
      self,
    ));
  }

  late final _playback_device_get_statePtr = _lookup<
          ffi.NativeFunction<ffi.UnsignedInt Function(ffi.Pointer<ffi.Void>)>>(
      'playback_device_get_state');
  late final _playback_device_get_state = _playback_device_get_statePtr
      .asFunction<int Function(ffi.Pointer<ffi.Void>)>();

  /// Creates a waveform generator with the specified parameters.
  ffi.Pointer<ffi.Void> waveform_create(
    pcm_format_t pcmFormat,
    Dartu_int32_t channels,
    int sampleRate,
    waveform_type_t waveformType,
    double amplitude,
    double frequency,
  ) {
    return _waveform_create(
      pcmFormat.value,
      channels,
      sampleRate,
      waveformType.value,
      amplitude,
      frequency,
    );
  }

  late final _waveform_createPtr = _lookup<
      ffi.NativeFunction<
          ffi.Pointer<ffi.Void> Function(ffi.UnsignedInt, u_int32_t, ffi.Uint32,
              ffi.UnsignedInt, ffi.Double, ffi.Double)>>('waveform_create');
  late final _waveform_create = _waveform_createPtr.asFunction<
      ffi.Pointer<ffi.Void> Function(int, int, int, int, double, double)>();

  /// Destroys a waveform generator and releases its resources.
  void waveform_destroy(
    ffi.Pointer<ffi.Void> self,
  ) {
    return _waveform_destroy(
      self,
    );
  }

  late final _waveform_destroyPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>(
          'waveform_destroy');
  late final _waveform_destroy =
      _waveform_destroyPtr.asFunction<void Function(ffi.Pointer<ffi.Void>)>();

  /// Reads PCM frames from the waveform generator.
  void waveform_read_pcm_frames_with_buffer(
    ffi.Pointer<ffi.Void> self,
    ffi.Pointer<ffi.Void> pFramesOut,
    int framesCount,
    ffi.Pointer<ffi.Uint64> pFramesRead,
  ) {
    return _waveform_read_pcm_frames_with_buffer(
      self,
      pFramesOut,
      framesCount,
      pFramesRead,
    );
  }

  late final _waveform_read_pcm_frames_with_bufferPtr = _lookup<
          ffi.NativeFunction<
              ffi.Void Function(ffi.Pointer<ffi.Void>, ffi.Pointer<ffi.Void>,
                  ffi.Uint64, ffi.Pointer<ffi.Uint64>)>>(
      'waveform_read_pcm_frames_with_buffer');
  late final _waveform_read_pcm_frames_with_buffer =
      _waveform_read_pcm_frames_with_bufferPtr.asFunction<
          void Function(ffi.Pointer<ffi.Void>, ffi.Pointer<ffi.Void>, int,
              ffi.Pointer<ffi.Uint64>)>();

  late final addresses = _SymbolAddresses(this);
}

class _SymbolAddresses {
  final ProMiniaudioBindings _library;
  _SymbolAddresses(this._library);
  ffi.Pointer<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>
      get audio_context_destroy => _library._audio_context_destroyPtr;
  ffi.Pointer<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<ffi.Void>)>>
      get waveform_destroy => _library._waveform_destroyPtr;
}

/// Enumeration to represent common audio sample formats.
enum pcm_format_t {
  /// Unknown or unsupported PCM format.
  pcm_format_unknown(0),

  /// Unsigned 8-bit integer.
  pcm_format_u8(1),

  /// Signed 16-bit integer.
  pcm_format_s16(2),

  /// Signed 24-bit integer.
  pcm_format_s24(3),

  /// Signed 32-bit integer.
  pcm_format_s32(4),

  /// 32-bit floating point.
  pcm_format_f32(5),

  /// Total number of supported PCM formats.
  pcm_format_count(6);

  final int value;
  const pcm_format_t(this.value);

  static pcm_format_t fromValue(int value) => switch (value) {
        0 => pcm_format_unknown,
        1 => pcm_format_u8,
        2 => pcm_format_s16,
        3 => pcm_format_s24,
        4 => pcm_format_s32,
        5 => pcm_format_f32,
        6 => pcm_format_count,
        _ => throw ArgumentError("Unknown value for pcm_format_t: $value"),
      };
}

/// Describes an audio data format.
final class audio_format_t extends ffi.Struct {
  /// The PCM sample format (e.g., 16-bit integer).
  @ffi.UnsignedInt()
  external int pcmFormatAsInt;

  pcm_format_t get pcmFormat => pcm_format_t.fromValue(pcmFormatAsInt);

  /// The number of audio channels (e.g., 1 for mono, 2 for stereo).
  @ffi.Uint32()
  external int channels;

  /// The sample rate in Hertz (e.g., 44100 Hz).
  @ffi.Uint32()
  external int sampleRate;
}

/// Provides detailed information about an audio device.
final class device_info_t extends ffi.Struct {
  /// Unique identifier for the device.
  external ffi.Pointer<ffi.Void> id;

  /// Null-terminated string representing the device name.
  @ffi.Array.multi([256])
  external ffi.Array<ffi.Char> name;

  /// Indicates whether this is the default device.
  @ffi.Bool()
  external bool isDefault;

  /// Number of supported audio formats.
  @ffi.Uint32()
  external int formatCount;

  /// Array of supported audio formats.
  @ffi.Array.multi([64])
  external ffi.Array<audio_format_t> audioFormats;
}

/// Defines the severity levels for log messages.
enum LogLevel {
  /// Debug level: Detailed information for debugging purposes.
  LOG_LEVEL_DEBUG(0),

  /// Info level: General informational messages.
  LOG_LEVEL_INFO(1),

  /// Warning level: Potential issues that are non-critical.
  LOG_LEVEL_WARNING(2),

  /// Error level: Critical issues requiring immediate attention.
  LOG_LEVEL_ERROR(3);

  final int value;
  const LogLevel(this.value);

  static LogLevel fromValue(int value) => switch (value) {
        0 => LOG_LEVEL_DEBUG,
        1 => LOG_LEVEL_INFO,
        2 => LOG_LEVEL_WARNING,
        3 => LOG_LEVEL_ERROR,
        _ => throw ArgumentError("Unknown value for LogLevel: $value"),
      };
}

/// Configuration structure for a playback device.
final class playback_config_t extends ffi.Struct {
  /// Number of audio channels (e.g., 2 for stereo).
  @ffi.Uint32()
  external int channels;

  /// Sample rate in Hertz (e.g., 44100 Hz).
  @ffi.Uint32()
  external int sampleRate;

  /// PCM format of the audio data (e.g., `pcm_format_s16`).
  @ffi.UnsignedInt()
  external int pcmFormatAsInt;

  pcm_format_t get pcmFormat => pcm_format_t.fromValue(pcmFormatAsInt);

  /// Maximum threshold for the ring buffer.
  @ffi.Size()
  external int rbMaxThreshold;

  /// Minimum threshold for the ring buffer.
  @ffi.Size()
  external int rbMinThreshold;

  /// Total size of the ring buffer in bytes.
  @ffi.Size()
  external int rbSizeInBytes;
}

/// Represents audio data to be pushed to a playback device.
final class playback_data_t extends ffi.Struct {
  /// Pointer to the audio data to be played.
  external ffi.Pointer<ffi.Void> pUserData;

  /// Size of the audio data in bytes.
  @ffi.Uint32()
  external int sizeInBytes;
}

/// Represents the states of an audio device.
enum device_state_t {
  /// The device is not initialized.
  device_state_uninitialized(0),

  /// The device is initialized but not started.
  device_state_stopped(1),

  /// The device is active and processing audio data.
  device_state_started(2),

  /// The device is transitioning from stopped to started.
  device_state_starting(3),

  /// The device is transitioning from started to stopped.
  device_state_stopping(4);

  final int value;
  const device_state_t(this.value);

  static device_state_t fromValue(int value) => switch (value) {
        0 => device_state_uninitialized,
        1 => device_state_stopped,
        2 => device_state_started,
        3 => device_state_starting,
        4 => device_state_stopping,
        _ => throw ArgumentError("Unknown value for device_state_t: $value"),
      };
}

/// Represents the types of waveforms that can be generated.
enum waveform_type_t {
  /// Sine waveform.
  waveform_type_sine(0),

  /// Square waveform.
  waveform_type_square(1),

  /// Triangle waveform.
  waveform_type_triangle(2),

  /// Sawtooth waveform.
  waveform_type_sawtooth(3);

  final int value;
  const waveform_type_t(this.value);

  static waveform_type_t fromValue(int value) => switch (value) {
        0 => waveform_type_sine,
        1 => waveform_type_square,
        2 => waveform_type_triangle,
        3 => waveform_type_sawtooth,
        _ => throw ArgumentError("Unknown value for waveform_type_t: $value"),
      };
}

typedef u_int32_t = ffi.UnsignedInt;
typedef Dartu_int32_t = int;
