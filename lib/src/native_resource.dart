part of 'library.dart';

/// Base class for native resources.
/// Provides a finalizer for releasing native resources.
/// Must be implemented by subclasses.
///
/// A `NativeFinalizer` is attached to the resource during its initialization.
/// The finalizer ensures that the native resource is released when the Dart
/// object is garbage collected (GC), if the `dispose` method is not called
/// explicitly.
///
/// Example:
/// ```dart
/// class MyResource extends NativeResource<MyResource> {
///   MyResource._(Pointer<MyResource> ptr) : super._(ptr);
///
///   @override
///   finalizer = NativeFinalizer(
///     _bindings.addresses.my_resource_destroy.cast(),
///   );
///
///   @override
///   void releaseResource() {
///     _bindings.my_resource_destroy(_resource);
///   }
/// }
/// ```
abstract class NativeResource<T extends NativeType> implements Finalizable {
  /// Initializes a native resource and attaches a finalizer.
  /// The finalizer will automatically release the resource if the
  /// Dart object is garbage collected (GC), unless [dispose] is called
  /// explicitly to release the resource beforehand.
  /// Throws an [ArgumentError] if [ptr] is `null`.
  NativeResource._(Pointer<T> ptr) : _resource = ptr {
    if (ptr == nullptr) {
      throw ArgumentError.notNull('ptr');
    }

    finalizer.attach(this, ptr.cast(), detach: this);
  }

  /// The finalizer for this resource.
  /// Must be implemented by subclasses.
  ///
  /// The finalizer ensures that the resource is properly released when
  /// the object becomes unreachable (GC). This behavior complements the
  /// explicit [dispose] method.
  ///
  /// Example:
  /// ```dart
  /// static final _finalizer = NativeFinalizer(
  ///   _bindings.addresses.function_destroy.cast(),
  /// );
  ///
  /// @override
  /// NativeFinalizer get finalizer => _finalizer;
  /// ```
  NativeFinalizer get finalizer;

  /// Pointer to the native resource.
  final Pointer<T> _resource;

  /// Tracks whether the resource has been finalized or explicitly disposed.
  bool get isFinalized => _isFinalized;
  bool _isFinalized = false;

  /// Disposes the resource and releases associated resources.
  /// Throws a [StateError] if the resource is already disposed.
  ///
  /// This method should be called explicitly when the resource is no longer
  /// needed to ensure timely release of native resources.
  ///
  /// If [dispose] is not called, the finalizer will release the resource
  /// automatically when the object is garbage collected.
  @mustCallSuper
  void dispose() {
    if (_isFinalized) {
      throw StateError('Resource already disposed');
    }

    _isFinalized = true;

    // Detach the finalizer to avoid double release.
    finalizer.detach(this);

    // Explicitly release the resource.
    releaseResource();
  }

  /// Releases the native resource.
  /// Must be implemented by subclasses to specify how the resource should be
  /// released.
  ///
  /// This method is called explicitly by [dispose], or implicitly by the
  /// finalizer if the object is garbage collected.
  ///
  /// Example:
  /// ```dart
  /// @override
  /// void releaseResource() {
  ///   _bindings.function_destroy(_resource);
  /// }
  /// ```
  @protected
  void releaseResource();
}
