part of 'library.dart';

/// Converts a native `Array<Char>` to a Dart string.
///
/// - [array]: The native array of characters.
/// - [maxLength]: The maximum length to process (default: 256). This ensures
///   safety in case of missing null terminator.
///
/// Returns a Dart string.
String arrayCharToString(Array<Char> array, {int maxLength = 256}) {
  // Ensure maxLength is positive
  if (maxLength <= 0) {
    throw ArgumentError('maxLength must be greater than zero');
  }

  final charCodes = <int>[];

  for (var i = 0; i < maxLength; i++) {
    final char = array[i];

    if (char == 0) {
      break; // Stop at null terminator
    }

    charCodes.add(char);
  }

  return String.fromCharCodes(charCodes);
}

/// Converts a Dart string to a `AutoFreePointer<Char>.
/// The returned pointer should not be freed manually. It will be released when
/// the associated `AutoFreePointer` is garbage collected.

/// - [string]: The Dart string to convert.
/// Returns a native `AutoFreePointer<Char>`.
AutoFreePointer<Char> stringToCharPointer(String string) {
  final pointer = malloc<Char>(string.length + 1);

  for (var i = 0; i < string.length; i++) {
    (pointer + i).value = string.codeUnitAt(i);
  }

  (pointer + string.length).value = 0;

  return AutoFreePointer._(pointer);
}

extension PlaybackConfigExt on PlaybackConfig {
  AutoFreePointer<playback_config_t> toNative() {
    final nativePlaybackConfig = malloc.allocate<playback_config_t>(
      sizeOf<playback_config_t>(),
    );

    nativePlaybackConfig.ref.channels = channels;
    nativePlaybackConfig.ref.sampleRate = sampleRate;
    nativePlaybackConfig.ref.pcmFormatAsInt = pcmFormat.index;
    nativePlaybackConfig.ref.rbMinThreshold = ringBufferMinThreshold;
    nativePlaybackConfig.ref.rbMaxThreshold = ringBufferMaxThreshold;
    nativePlaybackConfig.ref.rbSizeInBytes = ringBufferSizeInBytes;

    return AutoFreePointer._(
      nativePlaybackConfig,
    );
  }
}

extension PcmFormatExt on PcmFormat {
  pcm_format_t toNative() => pcm_format_t.values[index];
}

extension WaveformTypeExt on WaveformType {
  waveform_type_t toNative() => waveform_type_t.values[index];
}

extension LogLevelExt on FileLogLevel {
  log_level_t toNative() => log_level_t.values[index];
}

extension AudioDeviceTypeExt on AudioDeviceType {
  audio_device_type_t toNative() => audio_device_type_t.values[index];
}

base mixin AutoFreePointerHash<T extends NativeType> on AutoFreePointer<T> {

  /// A prime number used in hash computation.
  int get prime;

  /// Precomputed hash value.
  late final int hash = _calculateHash();

  /// The length of the data to hash.
  int get dataLeigh => 256;

  /// Calculates the hash value for the pointer data.
  int _calculateHash() {
    var hash = 0;

    final data = Pointer<Uint8>.fromAddress(_resource.address);

    for (var i = 0; i < dataLeigh; i++) {
      final value = data[i];
      hash = (hash * prime + value) & 0xFFFFFFFF;

      if (value == 0) {
        break;
      }
    }

    return hash;
  }
}
