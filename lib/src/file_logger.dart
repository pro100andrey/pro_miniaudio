part of 'library.dart';

/// Singleton class for managing file logging.
///
/// The `FileLogger` class provides methods to enable and disable logging to a
/// file.
/// You can specify the log file path and the desired log level.
final class FileLogger {
  /// Private constructor to enforce singleton usage.
  FileLogger._();

  /// Static instance of the `FileLogger` class.
  ///
  /// Use this instance to interact with the logger.
  static final instance = FileLogger._();

  /// Enables logging to a specified file.
  ///
  /// This method initializes the file logging system, sets the log file path,
  /// and specifies the log level for filtering log messages.
  ///
  /// ### Parameters
  /// - `path`  Path to the log file.
  /// - `level`  Optional. Specifies the logging level. Default is
  /// `FileLogLevel.info`.
  ///
  /// ### Example
  /// ```dart
  /// import 'package:flutter/material.dart';
  /// import 'package:path_provider/path_provider.dart';
  /// import 'package:pro_miniaudio/pro_miniaudio.dart';
  ///
  /// import 'app.dart';
  ///
  /// Future<void> main() async {
  ///   WidgetsFlutterBinding.ensureInitialized();
  ///
  ///   final temp = await getTemporaryDirectory();
  ///   final path = '${temp.path}/log.txt';
  ///   FileLogger.instance.enable(path, level: FileLogLevel.debug);
  ///
  ///   debugPrint('Log path: $path');
  ///
  ///   runApp(const MyApp());
  /// }
  /// ```
  void enable(String path, {FileLogLevel level = FileLogLevel.info}) {
    final filePath = stringToCharPointer(path);
    _bindings
      ..set_log_to_file_enabled(true)
      ..set_log_level(level.toNative())
      ..init_file_log(filePath.ensureIsNotFinalized());
  }

  /// Disables logging to a file.
  ///
  /// This method stops the file logging system and releases any associated
  /// resources.
  ///
  /// ### Example
  /// ```dart
  /// FileLogger.instance.disable();
  /// ```
  void disable() {
    _bindings
      ..set_log_to_file_enabled(false)
      ..close_file_log();
  }
}
