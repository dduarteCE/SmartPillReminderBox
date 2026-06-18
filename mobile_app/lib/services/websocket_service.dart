import 'package:web_socket_channel/web_socket_channel.dart';

class WebSocketService {

  WebSocketChannel? channel;

  void connect(String ip) {

    channel =
        WebSocketChannel.connect(
          Uri.parse(
            "ws://$ip/ws/events",
          ),
        );
  }

  Stream get stream =>
      channel!.stream;

  void disconnect() {
    channel?.sink.close();
  }
}