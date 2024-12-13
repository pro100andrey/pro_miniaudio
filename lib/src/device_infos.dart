part of 'library.dart';

class DeviceInfos extends NativeResource<device_infos_t> {
  DeviceInfos._(super.ptr) : super._(externalSize: sizeOf<device_infos_t>());

  @override
  NativeFinalizer get finalizer => Library._deviceInfosFinalizer;

  int get count => _resource.ref.count;

  List<DeviceInfo> getList() {
    final devices = _resource.ref.list;

    return List.generate(count, (i) {
      final nativeDeviceInfo = devices[i];

      final idPointer = malloc.allocate<device_id>(sizeOf<device_id>())
        ..ref = nativeDeviceInfo.id;

      final id = DeviceId._(idPointer);

      return DeviceInfo(
        id: id,
        name: arrayCharToString(nativeDeviceInfo.name),
        isDefault: nativeDeviceInfo.isDefault,
      );
    });
  }

  @override
  void releaseResource() {
    _bindings.audio_context_device_infos_destroy(_resource);
  }
}
