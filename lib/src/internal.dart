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

extension SampleFormatExt on SampleFormat {
  sample_format_t toNative() => sample_format_t.values[index];
}

extension WaveformTypeExt on WaveformType {
  waveform_type_t toNative() => waveform_type_t.values[index];
}
