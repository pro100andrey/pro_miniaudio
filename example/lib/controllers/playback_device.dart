import 'dart:async';

import 'package:pro_miniaudio/pro_miniaudio.dart';

const _durationInMs = 100;

int _calculateFamesCount({
  required int sampleRate,
  required int durationInMs,
}) =>
    sampleRate * durationInMs ~/ 1000;

class PlaybackWaveformDevice {
  PlaybackWaveformDevice({
    required Object deviceId,
    required List<SupportedFormat> supportedFormats,
    required SupportedFormat supportedFormat,
    required this.name,
  })  : _formats = supportedFormats,
        _supportedFormat = supportedFormat {
    final bpf = _supportedFormat.bytesPerFrame;

    final frameCount = _calculateFamesCount(
      sampleRate: _supportedFormat.sampleRate,
      durationInMs: _durationInMs,
    );

    final dataSizeInBytes = frameCount * bpf;
    final bufferSizeInBytes = dataSizeInBytes * 5;

    _playbackDevice = PlaybackDevice(
      deviceId: deviceId,
      bufferSizeInBytes: bufferSizeInBytes,
      format: _supportedFormat,
    );

    _applyConfig();
  }

  final String name;

  PlaybackDevice? _playbackDevice;

  List<SupportedFormat> get formats => _formats;
  List<SupportedFormat> _formats;

  WaveformType get waveformType => _waveformType;
  WaveformType _waveformType = WaveformType.sine;

  SupportedFormat get supportedFormat => _supportedFormat;
  SupportedFormat _supportedFormat;

  double get frequency => _frequency;
  double _frequency = 500;

  double get amplitude => _amplitude;
  double _amplitude = 0.4;

  Waveform? _waveform;

  bool get isPlaying => _timer != null;

  void setSupportedFormat(SupportedFormat format) {
    if (format == _supportedFormat) {
      return;
    }

    _supportedFormat = format;

    _applyConfig();
  }

  void setFormats(List<SupportedFormat> formats) {
    _formats = formats;

    if (!_formats.contains(_supportedFormat)) {
      _supportedFormat = _formats.first;
    }

    _applyConfig();
  }

  void setFrequency(double value) {
    if (value == _frequency) {
      return;
    }

    _frequency = value;

    _applyConfig();
  }

  void setAmplitude(double value) {
    if (value == _amplitude) {
      return;
    }

    _amplitude = value;

    _applyConfig();
  }

  void setWaveformType(WaveformType type) {
    if (type == _waveformType) {
      return;
    }

    _waveformType = type;

    _applyConfig();
    _playbackDevice?.resetBuffer();
  }

  void _applyConfig() {
    _waveform?.dispose();
    _waveform = null;

    final config = switch (_waveformType) {
      WaveformType.sine => WaveformSineConfig(
          format: _supportedFormat.format,
          sampleRate: _supportedFormat.sampleRate,
          channels: _supportedFormat.channels,
          amplitude: _amplitude,
          frequency: _frequency,
        ),
      WaveformType.square => WaveformSquareConfig(
          format: _supportedFormat.format,
          sampleRate: _supportedFormat.sampleRate,
          channels: _supportedFormat.channels,
          amplitude: _amplitude,
          frequency: _frequency,
        ),
      WaveformType.triangle => WaveformTriangleConfig(
          format: _supportedFormat.format,
          sampleRate: _supportedFormat.sampleRate,
          channels: _supportedFormat.channels,
          amplitude: _amplitude,
          frequency: _frequency,
        ),
      WaveformType.sawtooth => WaveformSawtoothConfig(
          format: _supportedFormat.format,
          sampleRate: _supportedFormat.sampleRate,
          channels: _supportedFormat.channels,
          amplitude: _amplitude,
          frequency: _frequency,
        ),
    };

    _waveform = Waveform(config: config);
  }

  void dispose() {
    _playbackDevice?.dispose();
    _playbackDevice = null;

    _waveform?.dispose();
    _waveform = null;

    if (_timer != null) {
      _timer!.cancel();
      _timer = null;
    }
  }

  Timer? _timer;

  void play() {
    assert(_waveform != null, 'Waveform is null');
    assert(_playbackDevice != null, 'Playback device is null');

    _playbackDevice!.start();

    _timer = Timer.periodic(
      const Duration(milliseconds: 100),
      (timer) {
        assert(_waveform != null, 'Waveform is null');
        assert(_playbackDevice != null, 'Playback device is null');
        final frameCount = _calculateFamesCount(
          sampleRate: _supportedFormat.sampleRate,
          durationInMs: _durationInMs,
        );

        final result = _waveform!.readWaveformPcmFrames(
          frameCount: frameCount,
        );

        _playbackDevice!.pushBuffer(
          buffer: result.frames,
          framesCount: result.framesRead,
        );
      },
    );
  }

  void stop() {
    _playbackDevice?.stop();
    _timer?.cancel();
    _timer = null;
  }
}
