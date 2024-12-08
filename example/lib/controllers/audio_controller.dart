import 'package:collection/collection.dart';
import 'package:flutter/foundation.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

import 'playback_device.dart';

class AudioController extends ChangeNotifier {

  final _context = Context();

  bool get dataIsRefreshed => _deviceInfos != null;

  ({List<DeviceInfo> capture, List<DeviceInfo> playback}) get devicesInfos =>
      _deviceInfos ?? kEmptyDevicesInfos;

  ({List<DeviceInfo> capture, List<DeviceInfo> playback})? _deviceInfos;

  DeviceInfo? _selectedPlaybackDevice;
  DeviceInfo? get selectedPlaybackDevice => _selectedPlaybackDevice;

  DeviceInfo? _selectedCaptureDevice;
  DeviceInfo? get selectedCaptureDevice => _selectedCaptureDevice;

  SupportedFormat? _selectedPlaybackFormat;
  SupportedFormat? get selectedPlaybackFormat => _selectedPlaybackFormat;

  SupportedFormat? _selectedCaptureFormat;
  SupportedFormat? get selectedCaptureFormat => _selectedCaptureFormat;

  void refreshDevices() {
    final result = _context.refreshAndReturnDevices();

    if (listEquals(result.playback, _deviceInfos?.playback) &&
        listEquals(result.capture, _deviceInfos?.capture)) {
      return;
    }

    if (!result.playback.contains(_selectedPlaybackDevice)) {
      _selectedPlaybackDevice = null;
    }

    if (!result.capture.contains(_selectedCaptureDevice)) {
      _selectedCaptureDevice = null;
    }

    _deviceInfos = result;

    _selectedPlaybackDevice ??= result.playback.isNotEmpty
        ? result.playback.firstWhere(
            (device) => device.isDefault,
            orElse: () => result.playback.first,
          )
        : null;

    _selectedCaptureDevice ??= result.capture.isNotEmpty
        ? result.capture.firstWhere(
            (device) => device.isDefault,
            orElse: () => result.capture.first,
          )
        : null;

    _selectedCaptureFormat ??=
        selectedCaptureDevice?.supportedFormats.firstOrNull;
    _selectedPlaybackFormat ??=
        selectedPlaybackDevice?.supportedFormats.firstOrNull;

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

  void selectPlaybackFormat(SupportedFormat format) {
    _selectedPlaybackFormat = format;

    notifyListeners();
  }

  void selectCaptureFormat(SupportedFormat format) {
    _selectedCaptureFormat = format;

    notifyListeners();
  }

  @override
  void dispose() {
    _context.dispose();

    super.dispose();
  }

  List<PlaybackWaveformDevice> playbackDevices = [];

  void createPlaybackDevice() {
    playbackDevices.add(
      PlaybackWaveformDevice(
        deviceId: _selectedPlaybackDevice!.id,
        context: _context,
        supportedFormats: selectedPlaybackDevice!.supportedFormats,
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
