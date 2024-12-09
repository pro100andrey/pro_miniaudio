import 'package:flutter/foundation.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

import 'playback_device.dart';

class AudioController extends ChangeNotifier {
  final _context = Context();

  bool get dataIsRefreshed => _deviceInfos != null;

  ({List<DeviceInfo> capture, List<DeviceInfo> playback}) get devicesInfos =>
      _deviceInfos ?? (capture: [], playback: []);

  ({List<DeviceInfo> capture, List<DeviceInfo> playback})? _deviceInfos;

  DeviceInfo? _selectedPlaybackDevice;
  DeviceInfo? get selectedPlaybackDevice => _selectedPlaybackDevice;

  DeviceInfo? _selectedCaptureDevice;
  DeviceInfo? get selectedCaptureDevice => _selectedCaptureDevice;

  void refreshDevices() {
    _context.refreshDevices();

    final playback = _context.playbackDeviceInfos;
    final capture = _context.captureDeviceInfos;

    if (listEquals(playback, _deviceInfos?.playback) &&
        listEquals(capture, _deviceInfos?.capture)) {
      return;
    }

    if (!playback.contains(_selectedPlaybackDevice)) {
      _selectedPlaybackDevice = null;
    }

    if (!capture.contains(_selectedCaptureDevice)) {
      _selectedCaptureDevice = null;
    }

    _deviceInfos = (capture: capture, playback: playback);

    _selectedPlaybackDevice ??= playback.isNotEmpty
        ? playback.firstWhere(
            (device) => device.isDefault,
            orElse: () => playback.first,
          )
        : null;

    _selectedCaptureDevice ??= capture.isNotEmpty
        ? capture.firstWhere(
            (device) => device.isDefault,
            orElse: () => capture.first,
          )
        : null;

    notifyListeners();
  }

  void selectPlaybackDevice(DeviceInfo device) {
    _selectedPlaybackDevice = device;

    notifyListeners();
  }

  void selectCaptureDevice(DeviceInfo device) {
    _selectedCaptureDevice = device;

    notifyListeners();
  }

  @override
  void dispose() {
    _context.dispose();

    super.dispose();
  }

  List<PlaybackWaveformDevice> playbackDevices = [];

  void createPlaybackDevice() {
    final deviceId = _selectedPlaybackDevice!.id;
    final supportedFormats = selectedPlaybackDevice!.supportedFormats;
    final supportedFormat = supportedFormats.first;

    playbackDevices.add(
      PlaybackWaveformDevice(
        deviceId: deviceId,
        supportedFormats: supportedFormats,
        supportedFormat: supportedFormat,
        name: 'Playback device ${playbackDevices.length + 1}',
      ),
    );

    notifyListeners();
  }

  void createCaptureDevice() {
    throw UnimplementedError();
  }

  void removePlaybackDevice(int index) {
    playbackDevices.removeAt(index).dispose();
    notifyListeners();
  }

  bool isPlaying(int index) => playbackDevices[index].isPlaying;

  void playPlaybackDevice(int index) {
    playbackDevices[index].play();
    notifyListeners();
  }

  void stopPlaybackDevice(int index) {
    playbackDevices[index].stop();
    notifyListeners();
  }

  void setPlaybackDeviceFormats(int index, List<SupportedFormat> formats) {
    playbackDevices[index].setFormats(formats);
    notifyListeners();
  }

  void setPlaybackDeviceSupportedFormat(int index, SupportedFormat format) {
    playbackDevices.removeAt(index).dispose();

    final device = PlaybackWaveformDevice(
      deviceId: _selectedPlaybackDevice!.id,
      supportedFormats: selectedPlaybackDevice!.supportedFormats,
      supportedFormat: format,
      name: 'Playback device ${playbackDevices.length + 1}',
    );

    playbackDevices.insert(index, device);

    playbackDevices[index].setSupportedFormat(format);
    notifyListeners();
  }

  void setPlaybackDeviceFrequency(int index, double value) {
    playbackDevices[index].setFrequency(value);
    notifyListeners();
  }

  void setPlaybackDeviceAmplitude(int index, double value) {
    playbackDevices[index].setAmplitude(value);
    notifyListeners();
  }

  void setPlaybackDeviceWaveformType(int index, WaveformType type) {
    playbackDevices[index].setWaveformType(type);
    notifyListeners();
  }
}
