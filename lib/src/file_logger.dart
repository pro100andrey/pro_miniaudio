part of 'library.dart';

final class FileLogger {
  FileLogger._();

  static final instance = FileLogger._();

  void enable(String path) {
    final filepath = stringToCharPointer(path);
    _bindings
      ..set_log_to_file_enabled(true)
      ..set_log_level(LogLevel.LOG_LEVEL_DEBUG)
      ..init_file_log(filepath);

    malloc.free(filepath);
  }

  void disable() {
    _bindings
      ..set_log_to_file_enabled(false)
      ..close_file_log();
  }
}
