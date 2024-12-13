part of 'library.dart';

/// Abstract base class for managing native resources.
///
/// This class provides an interface for working with native resources that
/// require explicit or automatic finalization. It uses a `NativeFinalizer` to
/// ensure that resources are released when they are no longer referenced,
/// preventing memory leaks.
///
/// Subclasses must implement the [releaseResource] method to define how the
/// resource should be released and specify the [finalizer] for cleanup.
///
/// Example usage:
/// ```dart
/// part of 'library.dart';
/// 
/// class MyResource extends NativeResource<MyResource> {
///   MyResource._(super.ptr) : super._();
///
///   @override
///   NativeFinalizer get finalizer => Library.resourceFinalizer;
///
///   @override
///   void releaseResource() {
///     _bindings.my_resource_destroy(_resource);
///   }
/// }
/// `
///
/// ``
abstract class NativeResource<T extends NativeType> implements Finalizable {
  /// Constructs a native resource and attaches a finalizer.
  ///
  /// The finalizer ensures that the resource is released if [dispose] is not
  /// called explicitly. If [ptr] is `nullptr`, throws an [ArgumentError].
  ///
  /// - [ptr]: Pointer to the native resource.
  /// - [externalSize]: Optional size of the external resource for GC tracking.
  NativeResource._(Pointer<T> ptr, {int? externalSize}) : _resource = ptr {
    if (ptr == nullptr) {
      throw ArgumentError.notNull('ptr');
    }

    finalizer.attach(
      this,
      ptr.cast(),
      detach: this,
      externalSize: externalSize,
    );
  }

  /// Finalizer used to release the native resource.
  ///
  /// Subclasses must override this property to provide a finalizer
  /// appropriate for their specific resource type.
  NativeFinalizer get finalizer;

  /// Pointer to the native resource.
  final Pointer<T> _resource;

  /// Indicates whether the resource has been disposed or finalized.
  bool get isFinalized => _isFinalized;
  bool _isFinalized = false;

  /// Releases the resource manually.
  ///
  /// Throws a [StateError] if the resource is already finalized.
  /// If this method is not called, the finalizer will release the resource
  /// when the object is garbage collected.
  @mustCallSuper
  void dispose() {
    if (_isFinalized) {
      throw StateError('Resource already disposed');
    }

    // Detach the finalizer to avoid double release.
    finalizer.detach(this);

    // Explicitly release the resource.
    releaseResource();

    // Mark the resource as finalized.
    _isFinalized = true;
  }

  /// Defines how the native resource should be released.
  ///
  /// Subclasses must implement this method to specify how to release the
  /// resource. It is called explicitly by [dispose] or implicitly by the
  /// finalizer.
  void releaseResource();

  /// Ensures that the resource is not finalized before accessing it.
  ///
  /// Throws a [StateError] if the resource has been finalized or is invalid.
  Pointer<T> ensureIsNotFinalized() {
    if (_isFinalized || _resource == nullptr) {
      throw StateError('Resource is finalized');
    }
    return _resource;
  }
}

/// Simplified resource manager for native resources.
///
/// This class is a lightweight alternative to [NativeResource] that does not
/// use a finalizer. Resources must be manually released by calling [dispose].
abstract class ManagedResource<T extends NativeType> {
  /// Constructs a managed resource.
  ///
  /// Throws an [ArgumentError] if [ptr] is `nullptr`.
  ManagedResource._(Pointer<T> ptr) : _resource = ptr {
    if (ptr == nullptr) {
      throw ArgumentError.notNull('ptr');
    }
  }

  /// Pointer to the native resource.
  final Pointer<T> _resource;

  /// Indicates whether the resource has been disposed.
  bool get isFinalized => _isFinalized;
  bool _isFinalized = false;

  /// Releases the resource manually.
  ///
  /// Throws a [StateError] if the resource is already finalized.
  @mustCallSuper
  void dispose() {
    if (_isFinalized) {
      throw StateError('Resource already disposed');
    }

    // Explicitly release the resource.
    releaseResource();

    // Mark the resource as finalized.
    _isFinalized = true;
  }

  /// Defines how the native resource should be released.
  ///
  /// Subclasses must implement this method to specify resource cleanup logic.
  void releaseResource();

  /// Ensures that the resource is valid and not finalized.
  ///
  /// Throws a [StateError] if the resource has been finalized or is invalid.
  Pointer<T> ensureIsNotFinalized() {
    if (_isFinalized || _resource == nullptr) {
      throw StateError('Resource is finalized');
    }
    return _resource;
  }
}

/// Lightweight wrapper for native resources.
///
/// Automatically frees memory allocated via `malloc` when the Dart object
/// is garbage collected.
class WrappedResource<T extends NativeType> {
  /// Constructs a wrapped resource.
  ///
  /// Throws an [ArgumentError] if [ptr] is `nullptr`.
  WrappedResource._(Pointer<T> ptr) : _resource = ptr {
    if (ptr == nullptr) {
      throw ArgumentError.notNull('ptr');
    }
    _finalizer.attach(this, ptr, detach: this);
  }

  /// Finalizer for freeing memory.
  static final Finalizer<Pointer> _finalizer = Finalizer((ptr) {
    malloc.free(ptr);
  });

  /// Pointer to the native resource.
  final Pointer<T> _resource;

  /// Ensures that the resource is valid and not finalized.
  ///
  /// Throws a [StateError] if the resource is invalid.
  Pointer<T> ensureIsNotFinalized() {
    if (_resource == nullptr) {
      throw StateError('Resource is finalized');
    }
    return _resource;
  }
}
