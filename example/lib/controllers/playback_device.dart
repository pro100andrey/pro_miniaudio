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
    required this.deviceInfo,
    required AudioFormat audioFormat,
  }) : _audioFormat = audioFormat {
    final bpf = audioFormat.bytesPerFrame;

    final frameCount = _calculateFamesCount(
      sampleRate: audioFormat.sampleRate,
      durationInMs: _durationInMs,
    );

    final dataSizeInBytes = frameCount * bpf;
    final bufferSizeInBytes = dataSizeInBytes * 5;

    _playbackDevice = PlaybackDevice(
      deviceInfo: deviceInfo,
      bufferSizeInBytes: bufferSizeInBytes,
      audioFormat: audioFormat,
    );

    _applyConfig();
  }

  final DeviceInfo deviceInfo;
  late final PlaybackDevice _playbackDevice;

  WaveformType get waveformType => _waveformType;
  WaveformType _waveformType = WaveformType.sine;

  AudioFormat get audioFormat => _audioFormat;
  AudioFormat _audioFormat;

  double get frequency => _frequency;
  double _frequency = 500;

  double get amplitude => _amplitude;
  double _amplitude = 0.4;

  Waveform? _waveform;

  bool get isPlaying => _timer != null;

  void setAudioFormat(AudioFormat format) {
    if (format == _audioFormat) {
      return;
    }

    _audioFormat = format;

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
    _playbackDevice.resetBuffer();
  }

  void _applyConfig() {
    _waveform?.dispose();
    _waveform = null;

    final sampleFormat = _audioFormat.sampleFormat;
    final sampleRate = _audioFormat.sampleRate;
    final channels = _audioFormat.channels;

    final config = switch (_waveformType) {
      WaveformType.sine => WaveformSineConfig(
          sampleFormat: sampleFormat,
          sampleRate: sampleRate,
          channels: channels,
          amplitude: _amplitude,
          frequency: _frequency,
        ),
      WaveformType.square => WaveformSquareConfig(
          sampleFormat: sampleFormat,
          sampleRate: sampleRate,
          channels: channels,
          amplitude: _amplitude,
          frequency: _frequency,
        ),
      WaveformType.triangle => WaveformTriangleConfig(
          sampleFormat: sampleFormat,
          sampleRate: sampleRate,
          channels: channels,
          amplitude: _amplitude,
          frequency: _frequency,
        ),
      WaveformType.sawtooth => WaveformSawtoothConfig(
          sampleFormat: sampleFormat,
          sampleRate: sampleRate,
          channels: channels,
          amplitude: _amplitude,
          frequency: _frequency,
        ),
    };

    _waveform = Waveform(config: config);
  }

  void dispose() {
    _playbackDevice.dispose();

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

    _playbackDevice.start();

    _timer = Timer.periodic(
      const Duration(milliseconds: 100),
      (timer) {
        assert(_waveform != null, 'Waveform is null');
        final frameCount = _calculateFamesCount(
          sampleRate: _audioFormat.sampleRate,
          durationInMs: _durationInMs,
        );

        final result = _waveform!.readWaveformPcmFrames(
          frameCount: frameCount,
        );

        _playbackDevice.pushBuffer(
          buffer: result.frames,
          framesCount: result.framesRead,
        );
      },
    );
  }

  void stop() {
    _playbackDevice.stop();
    _timer?.cancel();
    _timer = null;
  }
}
