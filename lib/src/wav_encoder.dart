part of 'library.dart';

final class WavEncoder extends NativeResource<Void> {
  factory WavEncoder({
    required String filePath,
    required WavEncoderConfig config,
  }) {
    final filePathPtr = stringToCharPointer(filePath);
    final configPtr = config.toNative().ensureIsNotFinalized();
    final wavEncoder = WavEncoder._(
      _bindings.encoder_create(
        filePathPtr.ensureIsNotFinalized(),
        configPtr,
      ),
      config,
    );

    return wavEncoder;
  }

  /// Internal constructor.
  ///
  /// This is used internally by the factory constructor and should not
  /// be called directly.
  WavEncoder._(super.ptr, this.config) : super._();

  /// Creates a new [WavEncoder] instance with the specified configuration.
  final WavEncoderConfig config;

  @protected
  @override
  NativeFinalizer get finalizer => Library._wavEncoderFinalizer;

  @protected
  @override
  void releaseResource() => _bindings.encoder_destroy(
        ensureIsNotFinalized(),
      );
}
