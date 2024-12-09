part of 'library.dart';

final class FileLogger {
  FileLogger._();

  static final instance = FileLogger._();

  void enable(String path, {FileLogLevel level = FileLogLevel.info}) {
    final filepath = stringToCharPointer(path);
    _bindings
      ..set_log_to_file_enabled(true)
      ..set_log_level(level.toNative())
      ..init_file_log(filepath);

    malloc.free(filepath);
  }

  void disable() {
    _bindings
      ..set_log_to_file_enabled(false)
      ..close_file_log();
  }
}
